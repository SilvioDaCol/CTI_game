#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdbool.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <locale.h>

#define     FPS                     60
#define     TELA_LARGURA            1920
#define     TELA_ALTURA             1080
#define     TOLERANCIA              5
#define     TEMPO_JOGO_SEGUNDOS     300
#define     TEMPO_EXPLOSAO          15  //tempo em segundos = TEMPO_EXPLOSAO/60
#define     TELA_MENU               0
#define     TELA_ESCOLHA            1
#define     TELA_JOGO               2
#define     TELA_PAUSE              3
#define     TELA_RANKING            4
#define     TELA_FIM_TEMPO          5
#define     NUM_JOGADORES           10
#define     TAMANHO_NOME_PLAYER     10
#define     FAIXA1_POS_Y            362
#define     FAIXA2_POS_Y            718
#define     TAMANHO_LINHA_ALGORTIMO 200
#define     QTDE_LINHAS_ALGORITMO   16
#define     TUNEL_1                 0
#define     TUNEL_2                 1
#define     TUNEL_3                 2
#define     VELOCIDADE_MINIMA       5
#define     VELOCIDADE_MAXIMA       10

//----------------------------------------------------DECLARAÇÃO DE VARIÁVEIS GLOBAIS
// Ponteiro representando a janela principal
ALLEGRO_DISPLAY *janela = NULL;
//Poteiro que recebe  o TIMER do programa
ALLEGRO_TIMER *timer = NULL;
//Matriz para receber as carros e seus respectivos estados de destruição
ALLEGRO_BITMAP *carroImagem[3][3];
//Estrutura que representa os objetos do jogo(Nave, tunel1áculos, botões e etc.)
typedef struct{
    int posX;
    int posY;
    int xColisao;
    int yColisao;
    int x1Colisao;
    int y1Colisao;
    int largura;
    int altura;
    int temp;
    float vel;
    float ang;
    bool ativo;
    int textColour_R;
    int textColour_G;
    int textColour_B;
    char texto[200];
    ALLEGRO_BITMAP *objetoBitmap;
    ALLEGRO_FONT *objetoFont;
}Objeto;
// Declaração dos objetos do jogo
Objeto backGround;
Objeto backGroundCidade;
Objeto backGroundMenu;
Objeto backGroundEscolha;
Objeto backGroundRanking;
Objeto backGroundPause;
Objeto backGroundFimTempo;
Objeto carro;
Objeto tunel1;
Objeto tunel2;
Objeto tunel3;
Objeto Quadro;
Objeto btPlay;
Objeto btExit;
Objeto btPlayAgain;
Objeto btQuit;
Objeto btCarro1;
Objeto btCarro2;
Objeto btCarro3;
Objeto btCenarioEspaco;
Objeto btCenarioCidade;
Objeto btPlayJogo;
Objeto btPlayPause;
Objeto btExitPause;
Objeto textTime;
Objeto textRank;
Objeto textScoreJogo;
Objeto textPlayer;
Objeto ultimaChance;
Objeto seta;


//Declara e inicializa Variaveis de controle do jogo
int numTestes = 0;
int numAcertos = 0;
int passTunel= 0; //Armazena numero do tunel que o carro passou
int proxLinha = 0;
char txtTunel_1[QTDE_LINHAS_ALGORITMO][TAMANHO_LINHA_ALGORTIMO];
char txtTunel_2[QTDE_LINHAS_ALGORITMO][TAMANHO_LINHA_ALGORTIMO];
char txtTunel_3[QTDE_LINHAS_ALGORITMO][TAMANHO_LINHA_ALGORTIMO];
char scriptTXT[QTDE_LINHAS_ALGORITMO][TAMANHO_LINHA_ALGORTIMO];
char ultChTexto[10];
bool sair = 0;
bool pause = 0;
bool jogoSalvo = 0;
int somaScore = 0;
int tempJogo=0;
int telaAtual=TELA_MENU;
char nomePlayer[TAMANHO_NOME_PLAYER]={"No DE R.A."};
int inicializacao = 4000;
int piscaCarro=0;
int tempTelaFimTempo = 0;
int tipoCarro = 0;
int estadoCarro = 0;
int carroClick = 0;
int cenarioClick = 0;
int rankingScore[NUM_JOGADORES],i;
char rankingPlayers[NUM_JOGADORES][50];
float velDesloc = 8;
int qtdeTestes = 0;

//Variaveis para controle do teclado
bool cima = false;
bool baixo = false;
bool esquerda = false;
bool direita = false;
bool acelera = false;
bool freia = false;
int indexNome = -1;
int caracterPendente = 0;  // 0 - Não há letra a ser lida   e 1 - Letra  aguardando ser lida
char caracter='\0';    // Armazena o caractere digitado
bool teclaEnter = false;
bool teclaApagar = false;


//Variaveis para controle do mouse
int posXMouse = 4000;
int posXMouseClick = 4000;
int posYMouse = 4000;
int posYMouseClick = 4000;

/*----------------------------------------------------
RECEBE UMA STRING LINHA E IDENTIFICA SEU ESPAÇO EM
BRANCO E AS 3 ALTERNATIVAS POSSÍVEIS, DEVOLVE LINHA
FORMATADA: "_ _ _ _"
----------------------------------------------------*/
bool identificaOpcao(char *txt,char *op1,char *op2,char *op3);
/*----------------------------------------------------
Randomiza ordem dos textos dos Tuneis. index se refere a qual
linha do algoritmo o programa esta.
----------------------------------------------------*/
void inverteOrdemTxt(int index);
/*----------------------------------------------------
ATRIBUI OS VALORES INICIAIS AS VARIÁVEIS RELACIONADAS
A TELA JOGO ANTES DE INICIAR UMA NOVA PARTIDA
----------------------------------------------------*/
void resetJogo();
/*----------------------------------------------------
VERIFICA SE HOUVE COLISAO ENTRE OS OBJETOS PASSADOS
POR PARÂMETRO E RETORNA VERDADEIRO CASO HOUVE, E FALSO
CASO NÃO HOUVE.
----------------------------------------------------*/
bool colisao(Objeto obj1, Objeto obj2);
/*----------------------------------------------------
VERIFICA SE O CURSOR DO MOUSE ESTÁ POSICIONADO SOBRE
O BOTÃO PASSADO POR PARÂMETRO E RETORNA 1 CASO ESTEJA
E 0 CASO NÃO ESTEJA
----------------------------------------------------*/
int mousePosicao(ALLEGRO_BITMAP *botao, int posXBotao, int posYBotao);
/*----------------------------------------------------
VERIFICA SE OCORREU UM CLICK NO BOTÃO PASSADO POR
PARÂMETRO
----------------------------------------------------*/
int mouseClick(ALLEGRO_BITMAP *botao, int posXBotao, int posYBotao);
/*----------------------------------------------------
CARREGA AS IMAGENS(BITMAPS) DO JOGO EM SEUS RESPECTIVOS
PONTEIROS
----------------------------------------------------*/
void loadBitmap();
/*----------------------------------------------------
EXIBE A TELA INICIAL COM AS OPÇÕES DE JOGAR E SAIR
----------------------------------------------------*/
void telaMenu();
/*----------------------------------------------------
EXIBE A TELA PARA ESCOLHER 1 ENTRE 3 OPÇÕES DE NAVE,
1 ENTRE DUAS OPÇÕES DE CENÁRIO E INFORMAR O NOME(NICK)
DO PLAYER
----------------------------------------------------*/
void telaEscolha();
/*----------------------------------------------------
EXIBE UMA TELA DE PAUSE COM AS OPÇÕES DE CONTINUAR
JOGANDO OU SAIR DO JOGO
----------------------------------------------------*/
void telaPause();
/*----------------------------------------------------
ABRE ARQUIVO E CARREGA OS DADOS DO MESMO NOS VETORES
PASSADOS POR PARÂMETRO.
CASO NÃO HAJA ARQUIVO, CRIA UM NOVO.
EM SEGUIDA FECHA O ARQUIVO.
----------------------------------------------------*/
int abreArquivoDisco(char rankingPlayersArq[][50], int numJogadores, int scoreArq[]);
/*----------------------------------------------------
ABRE O ARQUIVO EXISTENTE E SALVA OS DADOS DO JOGO
PASSADOS POR PARÂMETRO NO MESMO. EM SEGUIDA FECHA O
ARQUIVO.
----------------------------------------------------*/
int SalvaArquivoDisco(char rankingPlayersArq[][50], int numJogadores, int scoreArq[]);
/*----------------------------------------------------
RECEBE OS VETORES DE PONTUAÇÃO E NOMES DOS JOGADORES E
ORDENA AMBOS DA MAIOR PONTUAÇÃO PARA A MENOR UTILIZANDO
O ALGORITMO DE ORDENAÇÃO INSERTION SORT.
----------------------------------------------------*/
void ordenaRanking(int *ranking[], char nomeJogadores[][50]);
/*----------------------------------------------------
CARREGA O RANKING A PARTIR DO ARQUIVO, EXIBE O RANKING
ORDENADO E AS OPÇÕES DE JOGAR NOVAMENTE OU SAIR
----------------------------------------------------*/
void telaRanking();
/*----------------------------------------------------
DESENHA AS IMAGENS CORRESPONDENTES A TELA JOGO
----------------------------------------------------*/
void drawTelaJogo ();
/*----------------------------------------------------
EXECUTA A MECÂNICA E A LÓGICA DO JOGO, EXIBINDO OS OBJETOS
EM MOVIMENTO NA TELA.
----------------------------------------------------*/
void telaJogo();
/*----------------------------------------------------
EXIBE A MENSAGEM DE QUE ACABOU O TEMPO E CHAMA A TELA
RANKING.
----------------------------------------------------*/
void telaFimTempo();
/*----------------------------------------------------
ATRIBUI OS VALORES INICIAIS AS VARIÁVEIS GLOBAIS
RELACIONADAS A TODAS AS TELAS.
----------------------------------------------------*/
void inicializaObjetos();
/*----------------------------------------------------
FUNÇÃO PRINCIPAL: INICIALIZA OS PARÂMETRO DA ALLEGRO
NECESSÁRIOS PARA O FUNCIONAMENTO DO JOGO.
CALCULA RESOLUÇÃO DO JOGO EM FUNÇÃO DA RESOLUÇÃO DA TELA
DO COMPUTADOR UTILIZADO.
----------------------------------------------------*/
int main(void){
    //------------------------------------- inicializacao Bibl. ALLEGRO -------------------------------------
    al_init();
    //------------------------------------- Variaveis para ENVENTOS -------------------------------------
    //Variavel ponteiro para fila de enventos
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    //------------------------------------- inicializacao JANELAS e IMAGENS ---------------------------------
    // Inicializa o add-on para utilização de imagens
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();


    // Configura a janela

    ALLEGRO_MONITOR_INFO info;

	int res_x_comp, res_y_comp;

	al_get_monitor_info(0, &info);

	res_x_comp = info.x2 - info.x1;
	res_y_comp = info.y2 - info.y1;

	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	janela = al_create_display(res_x_comp, res_y_comp);

	float red_x = res_x_comp / (float) TELA_LARGURA;
	float red_y = res_y_comp / (float) TELA_ALTURA;

	ALLEGRO_TRANSFORM transformar;

	al_identity_transform(&transformar);
	al_scale_transform(&transformar, red_x, red_y);
	al_use_transform(&transformar);

    //------------------------------------- inicializacao EVENTOS---------------------------------
    //Inicializa variável timer com FPS definido
    timer = al_create_timer(1.0 / FPS);

    //inicializa addon do teclado
    al_install_keyboard();

    //inicializa addon do mouse
    al_install_mouse();

    // Atribui o cursor padrão do sistema para ser usado
    al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

    //Inicializa variável fila de eventos
    fila_eventos = al_create_event_queue();

    //registra  timer, teclado e mouse na fila de eventos
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    //inicia a contagem do timer
    al_start_timer(timer);

    //Cria variavel para receber o tipo de evento detectado
    ALLEGRO_EVENT evento;

    inicializaObjetos();
    loadBitmap(); //carrega imagens para o jogo

    while(!sair){
        //Aguarda pelo proximo evento e, quando ocorrer, pega o ultimo evento da fila e atribui a variavel evento
        al_wait_for_event(fila_eventos, &evento);

        //****************************************************************************
        //Se o evento for o timer...
        //****************************************************************************
        if(evento.type == ALLEGRO_EVENT_TIMER){

            if(telaAtual==TELA_MENU){
                telaMenu();
            }else if (telaAtual==TELA_ESCOLHA){
                telaEscolha();
            }else if(telaAtual==TELA_JOGO){
                telaJogo();
            }else if(telaAtual == TELA_PAUSE){
                telaPause();
            }else if(telaAtual == TELA_FIM_TEMPO){
                telaFimTempo();
            }else if(telaAtual == TELA_RANKING){
                telaRanking();
            }

            //Atualiza a tela
            al_flip_display();
        }

        //****************************************************************************
        //se o evento for PRESSIONAR uma tecla...
        //****************************************************************************
        if (evento.type == ALLEGRO_EVENT_KEY_DOWN){
            //verifica qual tecla foi pressionada
            switch(evento.keyboard.keycode){
                //seta para cima
                case ALLEGRO_KEY_UP:
                cima = 1;
                break;
                //seta para baixo
                case ALLEGRO_KEY_DOWN:
                baixo = 1;
                break;
                //seta para esquerda
                case ALLEGRO_KEY_LEFT:
                esquerda = 1;
                break;
                //seta para direita.
                case ALLEGRO_KEY_RIGHT:
                direita = 1;
                break;
                case ALLEGRO_KEY_S:
                acelera = 1;
                break;
                case ALLEGRO_KEY_A:
                freia = 1;
                break;
                //esc. sair=1 faz com que o programa saia do loop principal
                case ALLEGRO_KEY_ESCAPE:
                switch(telaAtual){
                    case TELA_MENU:
                    sair=1;
                    break;
                    case TELA_ESCOLHA:
                    sair=1;
                    break;
                    case TELA_JOGO:
                    telaAtual=TELA_PAUSE;
                    break;
                    case TELA_PAUSE:
                    telaAtual=TELA_JOGO;
                    break;
                    case TELA_RANKING:
                    sair = 1;
                    break;
                }
            case ALLEGRO_KEY_BACKSPACE:
            teclaApagar = 1;
            break;
            case ALLEGRO_KEY_ENTER:
            teclaEnter = 1;
            break;
            }
        }

        //****************************************************************************
        //se o evento for SOLTAR uma tecla...
        //****************************************************************************
        if (evento.type == ALLEGRO_EVENT_KEY_UP){
            //verifica qual tecla foi pressionada
            switch(evento.keyboard.keycode){
                //seta para cima
                case ALLEGRO_KEY_UP:
                cima = 0;
                break;
                //seta para baixo
                case ALLEGRO_KEY_DOWN:
                baixo = 0;
                break;
                //seta para esquerda
                case ALLEGRO_KEY_LEFT:
                esquerda = 0;
                break;
                //seta para direita.
                case ALLEGRO_KEY_RIGHT:
                direita = 0;
                break;
                case ALLEGRO_KEY_S:
                acelera = 0;
                break;
                case ALLEGRO_KEY_A:
                freia = 0;
                break;
                case ALLEGRO_KEY_BACKSPACE:
                teclaApagar = 0;
                break;
                case ALLEGRO_KEY_ENTER:
                teclaEnter = 0;
                break;
            }

        }
        //****************************************************************************
        //se o evento for uma tecla de texto...
        //https://www.rafaeltoledo.net/tutorial-allegro-5-14-entrada-de-texto/
        //****************************************************************************
        else if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
            caracter = (char)evento.keyboard.unichar;
            if((caracter >= 97)&&(caracter <= 122))caracter-=32;
            if((isalnum((int)caracter))||(caracter==8)||(caracter==32))caracterPendente = 1;
        }

        //****************************************************************************
        // Se o evento foi de movimentação do mouse
        //****************************************************************************
        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
            posXMouse = evento.mouse.x/red_x;
            posYMouse = evento.mouse.y/red_y;
        }

        // Ou se o evento foi um clique do mouse
        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            posXMouseClick = evento.mouse.x/red_x;
            posYMouseClick = evento.mouse.y/red_y;
            printf("\nX: %i",posXMouseClick);
            printf("\nY: %i\n\n",posYMouseClick);
        }

        //****************************************************************************
        //se o evento for Fechar Janela...
        //****************************************************************************
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                sair=1;
        }

        if(sair){
           int resp = al_show_native_message_box(janela,"Fechar","Deseja mesmo interromper a\naprendizagem ativa agora??  :=( ","",NULL,ALLEGRO_MESSAGEBOX_YES_NO);
            if (resp)sair=1;
            else sair=0;
        }
    }//Fecha while(sair)

    // Finaliza o timer, janela e fila_eventos
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}//Fecha void main()

bool identificaOpcao(char *txt,char *op1,char *op2,char *op3){
    bool ok = true;
    int i,j,ini = 999,pv1=0,pv2=0,fim=999,aux = 0;
    char txtPreOp[TAMANHO_LINHA_ALGORTIMO],txtPosOp[TAMANHO_LINHA_ALGORTIMO];
	//Identifica as demarcacoes
	for(i=0;i<strlen(txt);i++){
		if((txt[i]=='%')&&(txt[i+1]=='%')){
            if(ini==999){
                i += 2;
                ini = i;
            }else{
                fim = i;
                break;
            }
		}
		if((txt[i]==';')&&(txt[i+1]==';')&&(pv1 == 0)){
            pv1 = i;
            i += 2;
		}
		if((txt[i]==';')&&(txt[i+1]==';')&&(pv2 == 0)){
            pv2 = i;
            i += 1;
		}
		//Linha para debug
		//printf("\n\n**************\n%i\n%i\n%i\n%i\n*********************\n\n",ini,pv1,pv2,fim);
	}
	if((ini == 999) || (fim == 999))return false;
	//preenche as opcoes de acordo com as demarcacoes
	if(pv1>=ini){
        j=0;
        for(i=ini;i<pv1;i++){
            op1[j] = txt[i];
            j++;
        }
        op1[j] = '\0';
	}

	if(pv2>=pv1+2){
        j=0;
        for(i=pv1+2;i<pv2;i++){
            op2[j] = txt[i];
            j++;
        }
        op2[j] = '\0';
	}

	if(fim>=pv2+2){
        j=0;
        for(i=pv2+2;i<fim;i++){
            op3[j] = txt[i];
            j++;
        }
        op3[j] = '\0';
	}

    j=0;
	for(i=0;i<ini-2;i++){
        txtPreOp[j] = txt[i];
        j++;
	}
	txtPreOp[j] = '\0';

	j=0;
	for(i=fim+2;i<strlen(txt);i++){
        txtPosOp[j] = txt[i];
        j++;
	}
	txtPosOp[j] = '\0';

    strcat(txtPreOp, "_ _ _ _");
    strcat(txtPreOp, txtPosOp);
    strcpy(txt, txtPreOp);

	//Linha para debug
    //printf("\n\n**************\n%i\n%i\n%i\n%i\n*********************\n\n",ini,pv1,pv2,fim);
	return ok;
}

void inverteOrdemTxt(int index){
    int chave = (rand() % 4)+ 1;
    switch(chave){
    case 1:
        strcpy(tunel1.texto,txtTunel_1[index]);
        strcpy(tunel2.texto,txtTunel_2[index]);
        strcpy(tunel3.texto,txtTunel_3[index]);
        break;
    case 2:
        strcpy(tunel2.texto,txtTunel_1[index]);
        strcpy(tunel3.texto,txtTunel_2[index]);
        strcpy(tunel1.texto,txtTunel_3[index]);
        break;
    case 3:
        strcpy(tunel3.texto,txtTunel_1[index]);
        strcpy(tunel1.texto,txtTunel_2[index]);
        strcpy(tunel2.texto,txtTunel_3[index]);
        break;
    default:
        strcpy(tunel3.texto,txtTunel_1[index]);
        strcpy(tunel2.texto,txtTunel_2[index]);
        strcpy(tunel1.texto,txtTunel_3[index]);
        break;
    }
}

void resetJogo(){
    FILE *arq = NULL; //arquivo para carregar Algoritmos
    int i,j;//Variaveis para controle de iteracoes
    if (cenarioClick == 1){
        backGround.objetoBitmap = al_load_bitmap("img/back/1920/cenario/backgroundEspaco.jpg");
    }else if (cenarioClick ==2){
        backGround.objetoBitmap = al_load_bitmap("img/back/1920/cenario/backgroundCidade.jpg");
    }
    numAcertos = 0;
    numTestes = 0;
    passTunel= 0;
    proxLinha = 0;
    qtdeTestes = 0;
    somaScore = 0;
    tempJogo=0;
    inicializacao = 0;
    ultimaChance.ativo = 0;
    ultimaChance.temp = 0;

    backGround.posX=0;
    backGround.posY=0;

    carro.posX = 150;
    carro.posY=500;
    carro.vel= 5;
    velDesloc = rand()%5 + 6;
    carro.ang= 2*ALLEGRO_PI;
    estadoCarro = 0;
    carro.objetoBitmap=carroImagem[tipoCarro][estadoCarro];
    piscaCarro=0;
    carro.altura = al_get_bitmap_height(carro.objetoBitmap);
    carro.largura = al_get_bitmap_width(carro.objetoBitmap)-50;

    //tunel1.posX = backGround.posX + TELA_LARGURA;
    tunel1.posY = 0;
    tunel1.largura = al_get_bitmap_width(tunel1.objetoBitmap);
    tunel1.altura = al_get_bitmap_height(tunel1.objetoBitmap);
    strcpy(txtTunel_1," ");
    //(rand() % 4)+ 1;
    tunel1.posX = (rand() % tunel1.largura) + TELA_LARGURA + tunel1.largura*2;

    //tunel2.posX = backGround.posX + TELA_LARGURA;
    tunel2.posY = 358;
    tunel2.largura = al_get_bitmap_width(tunel2.objetoBitmap);
    tunel2.altura = al_get_bitmap_height(tunel2.objetoBitmap);
    strcpy(txtTunel_2," ");
    tunel2.posX = (rand() % tunel2.largura) + TELA_LARGURA + tunel2.largura*2;

    //tunel3.posX = backGround.posX + TELA_LARGURA;
    tunel3.posY = 718;
    tunel3.largura = al_get_bitmap_width(tunel3.objetoBitmap);
    tunel3.altura = al_get_bitmap_height(tunel3.objetoBitmap);
    strcpy(txtTunel_3," ");
    tunel3.posX = (rand() % tunel3.largura) + TELA_LARGURA + tunel3.largura*2;

    Quadro.largura = al_get_bitmap_width(Quadro.objetoBitmap);
    Quadro.altura = al_get_bitmap_height(Quadro.objetoBitmap);
    Quadro.posX = TELA_LARGURA - Quadro.largura;
    Quadro.posY = 0 ;

    seta.largura = al_get_bitmap_width(seta.objetoBitmap);
    seta.altura = al_get_bitmap_height(seta.objetoBitmap);
    seta.posX = TELA_LARGURA - Quadro.largura - 30;
    seta.posY = 60;
    seta.temp = 0;

    //Carrega scriptTXT
    arq = fopen("exercicios/algoritmo1.txt","r");
    if(arq==NULL){
        al_show_native_message_box(janela,"NAO HA EXERCICOS CADASTRADOS !!!","INSIRA UM ARQUIVO ALGORITMO1.TXT\nNA PASTA EXERCICIOS!","",NULL,ALLEGRO_MESSAGEBOX_WARN);
        telaAtual = TELA_ESCOLHA;
    }else{
        for(i=0;i<QTDE_LINHAS_ALGORITMO;i++){
            fgets(scriptTXT[i],TAMANHO_LINHA_ALGORTIMO,arq);
        }
    }
    fclose(arq);
    for(i=0;i<QTDE_LINHAS_ALGORITMO;i++){
        //Retira a quebra de linha da string e substitui por caracter nulo
        j=0;
        while(j<TAMANHO_LINHA_ALGORTIMO){
            if(scriptTXT[i][j]=='\n'){
                scriptTXT[i][j] = '\0';
                break;
            }
            j++;
        }

    }
    //Limpa os vetores txtTunel
    for(i=0;i<QTDE_LINHAS_ALGORITMO;i++){
        strcpy(txtTunel_1[i]," ");
        strcpy(txtTunel_2[i]," ");
        strcpy(txtTunel_3[i]," ");
    }
    //Preence os vetores txtTunel com as strings do arquivo
    for(i=0;i<QTDE_LINHAS_ALGORITMO;i++){
        if(identificaOpcao(scriptTXT[i],txtTunel_1[i],txtTunel_2[i],txtTunel_3[i])){
            printf("\nLinha %i: Teste identificado", i);
            numTestes++;
            printf("\n\t%s  ||  %s  ||  %s",txtTunel_1[i],txtTunel_2[i],txtTunel_3[i]);
        }else{
            printf("\nLinha %i: Teste nao identificado", i);
        }
    }
    //Randomiza ordem dos textos dos Tuneis
    inverteOrdemTxt(0);

}
bool colisao(Objeto obj1, Objeto obj2){
    bool ok = false;
    if((obj1.ativo)&&(obj2.ativo)){
        int posX1Obj1 = obj1.posX + obj1.largura + obj1.x1Colisao;
        int posY1Obj1 = obj1.posY + obj1.altura + obj1.y1Colisao;
        int posX1Obj2 = obj2.posX + obj2.largura + obj2.x1Colisao;
        int posY1Obj2 = obj2.posY + obj2.altura + obj2.y1Colisao;

        if ( ( ( obj1.posY+obj1.yColisao>=(obj2.posY+obj2.yColisao+TOLERANCIA))&&(obj1.posY+obj1.yColisao<=(posY1Obj2-TOLERANCIA))) || ((posY1Obj1>=(obj2.posY+obj2.yColisao+TOLERANCIA))&&(posY1Obj1<=(posY1Obj2-TOLERANCIA)))){
            if ( ( ( obj1.posX+obj1.xColisao>=(obj2.posX+obj2.xColisao+TOLERANCIA))&&(obj1.posX+obj1.xColisao<=(posX1Obj2-TOLERANCIA))) || ((posX1Obj1>=(obj2.posX+obj2.xColisao+TOLERANCIA))&&(posX1Obj1<=(posX1Obj2-TOLERANCIA)))){
                ok = true;
            }
        }
    }
    return ok;
}
int mousePosicao(ALLEGRO_BITMAP *botao, int posXBotao, int posYBotao){
    int posicao=0;
    if((posXMouse>=posXBotao)&&(posXMouse<=(posXBotao + al_get_bitmap_width(botao)))){
        if((posYMouse>=posYBotao)&&(posYMouse<=(posYBotao + al_get_bitmap_height(botao)))){
            posicao=1;
        }else{
            posicao=0;
        }
    }else{
        posicao=0;
    }
    return posicao;
}
int mouseClick(ALLEGRO_BITMAP *botao, int posXBotao, int posYBotao){
    int click=0;
    if((posXMouseClick>=posXBotao)&&(posXMouseClick<=(posXBotao + al_get_bitmap_width(botao)))){
        if((posYMouseClick>=posYBotao)&&(posYMouseClick<=(posYBotao + al_get_bitmap_height(botao)))){
            click=1;
            posXMouseClick=4000;
            posYMouseClick=4000;
        }else{
            click=0;
        }
    }else{
        click=0;
    }

    return click;
}
void loadBitmap(){

    // CARREGA BITMAPS
    backGround.objetoBitmap =            al_load_bitmap("img/back/1920/cenario/backGroundEspaco.jpg");
    backGroundCidade.objetoBitmap =      al_load_bitmap("img/back/1920/cenario/backGroundCidade.jpg");
    backGroundMenu.objetoBitmap =        al_load_bitmap("img/back/1920/menuInicial/menuInicial1920.jpg");
    backGroundEscolha.objetoBitmap =     al_load_bitmap("img/back/1920/telaEscolha/telaEscolha1920.jpg");
    backGroundPause.objetoBitmap =       al_load_bitmap("img/back/1920/telaPause/telaPause.jpg");
    backGroundRanking.objetoBitmap =     al_load_bitmap("img/back/1920/telaRanking/telaRanking.jpg");
    backGroundFimTempo.objetoBitmap =    al_load_bitmap("img/back/1920/telaFimTempo/TELA_FimTempo.jpg");
    carroImagem[0][0] =                  al_load_bitmap("img/objetos/carro1.png");
    carroImagem[0][1] =                  al_load_bitmap("img/objetos/carro1.png");
    carroImagem[0][2] =                  al_load_bitmap("img/objetos/carro1.png");
    carroImagem[1][0] =                  al_load_bitmap("img/objetos/carro.png");
    carroImagem[1][1] =                  al_load_bitmap("img/objetos/carro.png");
    carroImagem[1][2] =                  al_load_bitmap("img/objetos/carro.png");
    carroImagem[2][0] =                  al_load_bitmap("img/objetos/carro2.png");
    carroImagem[2][1] =                  al_load_bitmap("img/objetos/carro2.png");
    carroImagem[2][2] =                  al_load_bitmap("img/objetos/carro2.png");
    btCarro1.objetoBitmap =              al_load_bitmap("img/back/1920/telaEscolha/botaoInversoNaveZero.png");
    btCarro2.objetoBitmap =              al_load_bitmap("img/back/1920/telaEscolha/botaoInversoNaveUm.png");
    btCarro3.objetoBitmap =              al_load_bitmap("img/back/1920/telaEscolha/botaoInversoNaveDois.png");
    btCenarioEspaco.objetoBitmap =       al_load_bitmap("img/back/1920/telaEscolha/botaoInversoCenarioEspaco.png");
    btCenarioCidade.objetoBitmap =       al_load_bitmap("img/back/1920/telaEscolha/botaoInversoCenarioCidade.png");
    btPlayJogo.objetoBitmap =            al_load_bitmap("img/back/1920/telaEscolha/botaoInversoPlayJogo.png");
    tunel1.objetoBitmap =                al_load_bitmap("img/objetos/tunel_ajuste.png");
    tunel2.objetoBitmap =                al_load_bitmap("img/objetos/tunel_ajuste.png");
    tunel3.objetoBitmap =                al_load_bitmap("img/objetos/tunel_ajuste.png");
    Quadro.objetoBitmap =                al_load_bitmap("img/objetos/quadro.png");
    btPlay.objetoBitmap =                al_load_bitmap("img/back/1920/menuInicial/botaoInversoPlay.png");
    btExit.objetoBitmap =                al_load_bitmap("img/back/1920/menuInicial/botaoInversoExit.png");
    btPlayAgain.objetoBitmap =           al_load_bitmap("img/back/1920/telaRanking/botaoInversoPlayAgain.png");
    btQuit.objetoBitmap =                al_load_bitmap("img/back/1920/telaRanking/botaoInversoQuit.png");
    btPlayPause.objetoBitmap =           al_load_bitmap("img/back/1920/telaPause/botaoInversoPlayPause.png");
    btExitPause.objetoBitmap =           al_load_bitmap("img/back/1920/telaPause/botaoInversoExitPause.png");
    seta.objetoBitmap =                  al_load_bitmap("img/objetos/seta.jpg");

    // CARREGA FONTES TTF
    textTime.objetoFont =                al_load_font("fonte/alarm_clock/alarm_clock.ttf", 80, ALLEGRO_TTF_NO_KERNING);
    textRank.objetoFont =                al_load_font("fonte/cambriab.ttf", 30, ALLEGRO_TTF_NO_KERNING);
    Quadro.objetoFont =                  al_load_font("fonte/cambriab.ttf", 30, ALLEGRO_TTF_NO_KERNING);
    textPlayer.objetoFont =              al_load_font("fonte/alarm_clock/alarm_clock.ttf", 90, ALLEGRO_TTF_NO_KERNING);
    textScoreJogo.objetoFont =           al_load_font("fonte/alarm_clock/alarm_clock.ttf", 80, ALLEGRO_TTF_NO_KERNING);
    ultimaChance.objetoFont =            al_load_font("fonte/alarm_clock/alarm_clock.ttf", 80, ALLEGRO_TTF_NO_KERNING);
    tunel1.objetoFont =                  al_load_font("fonte/cambriab.ttf", 70, ALLEGRO_TTF_NO_KERNING);
    tunel2.objetoFont =                  al_load_font("fonte/cambriab.ttf", 70, ALLEGRO_TTF_NO_KERNING);
    tunel3.objetoFont =                  al_load_font("fonte/cambriab.ttf", 70, ALLEGRO_TTF_NO_KERNING);

}
void telaMenu(){//----------------------------------------------------FUNCAO RESPONSALVEL PELO MENU
     al_draw_bitmap(btPlay.objetoBitmap, btPlay.posX, btPlay.posY, 0);
     al_draw_bitmap(btExit.objetoBitmap, btExit.posX, btExit.posY, 0);
     al_draw_bitmap(backGroundMenu.objetoBitmap, 0, 0, 0);

     if(mouseClick(btPlay.objetoBitmap, btPlay.posX, btPlay.posY)){
        telaAtual=TELA_ESCOLHA;
     }else if(mouseClick(btExit.objetoBitmap,btExit.posX,btExit.posY)){
         sair=1;
     }

     if(mousePosicao(btPlay.objetoBitmap, btPlay.posX, btPlay.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btPlay.objetoBitmap, btPlay.posX, btPlay.posY, 0);
     }else if(mousePosicao(btExit.objetoBitmap,btExit.posX,btExit.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btExit.objetoBitmap, btExit.posX, btExit.posY, 0);
     }else{
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
     }

}
void telaEscolha(){//----------------------------------------------------FUNCAO RESPONSALVEL PELO MENU
     //Monta a telaEscolha
     al_draw_bitmap(backGroundEscolha.objetoBitmap, 0, 0, 0);
     al_draw_textf(textPlayer.objetoFont, al_map_rgb(255, 255, 0), 980, 631, ALLEGRO_ALIGN_CENTER, "%s",nomePlayer);

     if(indexNome==0){
        strcpy(nomePlayer,"No DE R.A.");
     }
     if(caracterPendente){
        if(indexNome<0){
            indexNome = 0;
            strcpy(nomePlayer," ");
        }
        if(teclaApagar){
            indexNome--;
            caracter=' ';
        }
        if(indexNome<TAMANHO_NOME_PLAYER-1){
            nomePlayer[indexNome]=caracter;
            indexNome++;
            nomePlayer[indexNome] = '\0';
        }
        if(teclaApagar){
            indexNome--;
        }
        caracterPendente=0;
     }

     //Funcoes do mouse
     if(mousePosicao(btCarro1.objetoBitmap, btCarro1.posX, btCarro1.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btCarro1.objetoBitmap,btCarro1.posX, btCarro1.posY, 0);
     }else if(mousePosicao(btCarro2.objetoBitmap, btCarro2.posX, btCarro2.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btCarro2.objetoBitmap,btCarro2.posX, btCarro2.posY, 0);
     }else if(mousePosicao(btCarro3.objetoBitmap, btCarro3.posX, btCarro3.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btCarro3.objetoBitmap,btCarro3.posX, btCarro3.posY, 0);
     }else if(mousePosicao(btCenarioEspaco.objetoBitmap,btCenarioEspaco.posX, btCenarioEspaco.posY)){
        al_draw_bitmap(btCenarioEspaco.objetoBitmap, btCenarioEspaco.posX+15, btCenarioEspaco.posY+5, 0);
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
     }else if(mousePosicao(btCenarioCidade.objetoBitmap, btCenarioCidade.posX, btCenarioCidade.posY)){
        al_draw_bitmap(btCenarioCidade.objetoBitmap, btCenarioCidade.posX, btCenarioCidade.posY,0);
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
     }else if (mousePosicao(btPlayJogo.objetoBitmap, btPlayJogo.posX, btPlayJogo.posY)){
        al_draw_bitmap(btPlayJogo.objetoBitmap, btPlayJogo.posX, btPlayJogo.posY, 0);
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
     } else{
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
     }


     if(mouseClick(btCarro1.objetoBitmap, btCarro1.posX, btCarro1.posY)){
        tipoCarro=0;
        carroClick = 1;
     }else if(mouseClick(btCarro2.objetoBitmap, btCarro2.posX, btCarro2.posY)){
        tipoCarro=1;
        carroClick = 2;
     }else if(mouseClick(btCarro3.objetoBitmap, btCarro3.posX, btCarro3.posY)){
        tipoCarro=2;
        carroClick = 3;
     }else if(mouseClick(btCenarioEspaco.objetoBitmap, btCenarioEspaco.posX, btCenarioEspaco.posY)){
        cenarioClick = 1;
        textTime.textColour_R = 255;
        textTime.textColour_G = 255;
        textTime.textColour_B = 0;
     }else if(mouseClick(btCenarioCidade.objetoBitmap, btCenarioCidade.posX, btCenarioCidade.posY)){
        cenarioClick = 2;
        textTime.textColour_R = 255;
        textTime.textColour_G = 20;
        textTime.textColour_B = 147;

     }else if(mouseClick(btPlayJogo.objetoBitmap, btPlayJogo.posX, btPlayJogo.posY)){
        if ((cenarioClick == 1)||(cenarioClick == 2)){
            if(strcmp(nomePlayer,"No DE R.A.")){
                telaAtual = TELA_JOGO;
                resetJogo();
            }else al_show_native_message_box(janela,"! ! ! ATENCAO ! ! !","NAO SE ESQUECA DE \nDIGITAR SEU REGISTRO ACADEMICO!!!","",NULL,ALLEGRO_MESSAGEBOX_WARN);
        }else al_show_native_message_box(janela,"! ! ! ATENCAO ! ! !","PARA PROSSEGUIR, \nESCOLHA UM CENARIO!","",NULL,ALLEGRO_MESSAGEBOX_WARN);
     }

     if (carroClick == 1){
        al_draw_bitmap(btCarro1.objetoBitmap, btCarro1.posX, btCarro1.posY,0);
     } else if (carroClick == 2) {
        al_draw_bitmap(btCarro2.objetoBitmap, btCarro2.posX, btCarro2.posY,0);
     } else if (carroClick == 3){
        al_draw_bitmap(btCarro3.objetoBitmap, btCarro3.posX, btCarro3.posY,0);
     }
     if (cenarioClick == 1){
        al_draw_bitmap(btCenarioEspaco.objetoBitmap, btCenarioEspaco.posX+15, btCenarioEspaco.posY+5,0);
     } else if (cenarioClick == 2){
        al_draw_bitmap(btCenarioCidade.objetoBitmap, btCenarioCidade.posX, btCenarioCidade.posY,0);
     }

}
void telaPause(){//---------------------------------------------------FUNCAO RESPONSALVEL PELO PAUSE
    al_draw_bitmap(backGroundPause.objetoBitmap, 0, 0, 0);



    if(mouseClick(btPlayPause.objetoBitmap, btPlayPause.posX, btPlayPause.posY)){
        telaAtual = TELA_JOGO;
     }else if(mouseClick(btExitPause.objetoBitmap,btExitPause.posX,btExitPause.posY)){
         telaAtual = TELA_RANKING;
     }

     if(mousePosicao(btPlayPause.objetoBitmap, btPlayPause.posX, btPlayPause.posY)){
        al_draw_bitmap(btPlayPause.objetoBitmap, btPlayPause.posX, btPlayPause.posY, 0);
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
     }else if(mousePosicao(btExitPause.objetoBitmap,btExitPause.posX,btExitPause.posY)){
        al_draw_bitmap(btExitPause.objetoBitmap, btExitPause.posX, btExitPause.posY, 0);
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
     }else{
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
     }

}
int abreArquivoDisco(char rankingPlayersArq[][50], int numJogadores, int scoreArq[]){
    FILE *arq;
    char texto[200];
    int result,i;

    printf("\nProcurando jogos salvos em Disco...");
    arq = fopen("savedGames/Jogos_Salvos.txt","r");
    if(arq == NULL) {
        printf("\nArquivo nao encontrado.\n    Criando novo arquivo...");
        //------------------------------------------------------------------------------------------
        for(i=0;i<NUM_JOGADORES;i++){
            strcpy(&rankingPlayersArq[i][0],"[VAZIO]");
            scoreArq[i]=0;
        }
        arq = fopen("savedGames/Jogos_Salvos.txt","w");
        if(arq == NULL) {
            printf("Problemas na ABERTURA do arquivo linha 261\n");
        }
        result = fprintf(arq,"Posicao:\tPontuacao:\tNome:\n");
        if(result < 0)printf("Erro na escrita\n");
        for(i=0;i<NUM_JOGADORES;i++){
            result = fprintf(arq,"%i º\t\t",i+1);
            result = fprintf(arq,"%i\t\t",scoreArq[i]);
            result = fprintf(arq,"%s\n",rankingPlayersArq[i]);
            if(result < 0)printf("Erro na escrita\n");
        }
        fclose(arq);

        arq = fopen("savedGames/Jogos_Salvos.txt","r");
        if(arq == NULL) printf("\nErro ao abrir novo arquivo...");
        else printf("\nNovo Arquivo Criado com sucesso!!!");

        //------------------------------------------------------------------------------------------
    }
    fgets(texto,200,arq);//Le cabecaolho e descarta

    for(i=0;i<NUM_JOGADORES;i++){
        fscanf(arq," %s", texto); //Lê a posicao e descarta
        fscanf(arq," %s", texto); //Lê o "º" e descarta
        fscanf(arq," %i", &scoreArq[i]); //Lê a pontuacao e armazena
        fgets(&rankingPlayersArq[i],50,arq);  //Lê o Nome do Player e armazena
    }

    fclose(arq);

    return 1;
}
int SalvaArquivoDisco(char rankingPlayersArq[][50], int numJogadores, int scoreArq[]){

    FILE *arq;
    int result,i;

    arq = fopen("savedGames/Jogos_Salvos.txt","w");
    if(arq == NULL) {
        printf("Problemas na ABERTURA do arquivo\n");
    }
    printf("\nSalvando os dados...");
    result = fprintf(arq,"Posicao:\tPontuacao:\tNome:\n");
    if(result < 0)printf("Erro na escrita\n");
    for(i=0;i<NUM_JOGADORES;i++){
        result = fprintf(arq,"%i º\t\t",i+1);
        result = fprintf(arq,"%i",scoreArq[i]);
        result = fprintf(arq,"%s\n",rankingPlayersArq[i]);
    }
    if(result < 0)printf("Erro na escrita\n");
    else printf("\n   Dados salvos com Sucesso!!!...");
    fclose(arq);

    return 1;
}
void ordenaRanking(int *ranking[], char nomeJogadores[][50]){
    int i,j,aux;
    char auxNome[50];

    for(i=1;i<NUM_JOGADORES;i++){
        if(ranking[i]>ranking[i-1]){
            aux = ranking[i];
            strcpy(auxNome,nomeJogadores[i]);
            ranking[i] = ranking [i-1];
            strcpy(nomeJogadores[i],nomeJogadores[i-1]);
            ranking[i-1] = aux;
            strcpy(nomeJogadores[i-1],auxNome);

            for(j=i-1;j>0;j--){
                if(ranking[j]>ranking[j-1]){
                    aux = ranking[j];
                    strcpy(auxNome,nomeJogadores[j]);
                    ranking[j] = ranking [j-1];
                    strcpy(nomeJogadores[j],nomeJogadores[j-1]);
                    ranking[j-1] = aux;
                    strcpy(nomeJogadores[j-1],auxNome);
                }else{
                    break;
                }
            }
        }
    }


}
void telaRanking(){//-------------------------------------------------FUNCAO RESPONSALVEL PELO RANKING
    char txtF[TAMANHO_NOME_PLAYER]; // Recebe Texto Formatado

    if(!jogoSalvo){
        // ABRE OU CRIA UM AQUIVO E COPIA VALORES SALVOS PARA OS ARRAYS DE NOME E PONTUACAO
        jogoSalvo = abreArquivoDisco(rankingPlayers,NUM_JOGADORES,rankingScore);
        // INSERE OS DADOS DO ÚLTIMO JOGO REALIZADO NO PRIMEIRO ESPAÇO DISPONIVEL DO ARRAY
        for(i=0;i<NUM_JOGADORES;i++){
            if(rankingScore[i]<somaScore){
                strcpy(&rankingPlayers[i],"\t\t");
                strcat(&rankingPlayers[i],&nomePlayer);
                strcat(&rankingPlayers[i],"\n");
                rankingScore[i] = somaScore;
                break;
            }
        }
        //ORDENA OS ARRAYS DA MAIOR PARA A MENOR PONTUAÇÃO (DE CIMA PARA BAIXO RESPECTIVAMENTE)
        ordenaRanking(rankingScore, rankingPlayers);

        //SALVA ARRAYS ATUALIZADOS NO ARQUIVO
        jogoSalvo = SalvaArquivoDisco(rankingPlayers,NUM_JOGADORES,rankingScore);
    }

    al_draw_bitmap(backGroundRanking.objetoBitmap, 0, 0, 0);

    //SCORE FINAL
    al_draw_textf(textScoreJogo.objetoFont, al_map_rgb(255, 255, 0), 270, 65, ALLEGRO_ALIGN_LEFT, "%i",somaScore);
    //RANKING PLAYERS
    int posYTextRanking = 180;
    for(i=0;i<NUM_JOGADORES;i++){
        strcpy(txtF, &rankingPlayers[i][2]);
        txtF[strlen(txtF)-1]='\0';
        txtF[TAMANHO_NOME_PLAYER-1]='\0';
        al_draw_text(textRank.objetoFont, al_map_rgb(255, 255, 0), 270, posYTextRanking, ALLEGRO_ALIGN_LEFT, txtF);
        al_draw_textf(textRank.objetoFont, al_map_rgb(255, 255, 0), 620, posYTextRanking, ALLEGRO_ALIGN_RIGHT, "%i", rankingScore[i]);
        posYTextRanking += 60;
    }

    if(mouseClick(btPlayAgain.objetoBitmap, btPlayAgain.posX, btPlayAgain.posY)){
        carroClick = 0;
        cenarioClick = 0;
        jogoSalvo = 0;
        strcpy(nomePlayer,"No DE R.A.");
        indexNome = -1;
        caracterPendente=0;
        caracter='\0';
        teclaApagar = 0;
        telaAtual=TELA_ESCOLHA;
     }else if(mouseClick(btQuit.objetoBitmap,btQuit.posX,btQuit.posY)){
         jogoSalvo = 0;
         sair=1;
     }

     if(mousePosicao(btPlayAgain.objetoBitmap, btPlayAgain.posX, btPlayAgain.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btPlayAgain.objetoBitmap, btPlayAgain.posX, btPlayAgain.posY, 0);
     }else if(mousePosicao(btQuit.objetoBitmap,btQuit.posX,btQuit.posY)){
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
        al_draw_bitmap(btQuit.objetoBitmap, btQuit.posX, btQuit.posY, 0);
     }else{
        al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
     }

}
void drawTelaJogo (){
    int i,posYTextQuadro = 60;
    //------------------------------------------------------------Desenha/Atualiza objetos na tela
    al_draw_bitmap(backGround.objetoBitmap, backGround.posX, backGround.posY, 0);
    if(carro.ativo)al_draw_rotated_bitmap(carro.objetoBitmap, carro.largura/2, carro.altura/2, carro.posX, carro.posY,carro.ang,0);
    if(tunel1.ativo){
        al_draw_bitmap(tunel1.objetoBitmap, tunel1.posX, tunel1.posY, 0);
        al_draw_text(tunel1.objetoFont, al_map_rgb(255, 255, 0), tunel1.posX+120, tunel1.posY+100, ALLEGRO_ALIGN_LEFT, tunel1.texto);
    }
    if(tunel2.ativo){
        al_draw_bitmap(tunel2.objetoBitmap, tunel2.posX, tunel2.posY, 0);
        al_draw_text(tunel2.objetoFont, al_map_rgb(255, 255, 0), tunel2.posX+120, tunel2.posY+100, ALLEGRO_ALIGN_LEFT, tunel2.texto);
    }
    if(tunel3.ativo){
        al_draw_bitmap(tunel3.objetoBitmap, tunel3.posX, tunel3.posY, 0);
        al_draw_text(tunel3.objetoFont, al_map_rgb(255, 255, 0), tunel3.posX+120, tunel3.posY+100, ALLEGRO_ALIGN_LEFT, tunel3.texto);
    }
    if(Quadro.ativo)al_draw_bitmap(Quadro.objetoBitmap, Quadro.posX, Quadro.posY, 0);
    if(seta.ativo)al_draw_bitmap(seta.objetoBitmap, seta.posX-10, seta.posY-10, 0);

    if(textScoreJogo.ativo)al_draw_textf(textScoreJogo.objetoFont, al_map_rgb(textTime.textColour_R, textTime.textColour_G, textTime.textColour_B), 20, 10, ALLEGRO_ALIGN_LEFT, "SCORE(%%): %i", somaScore);
    //if(textTime.ativo)al_draw_textf(textTime.objetoFont, al_map_rgb(textTime.textColour_R, textTime.textColour_G, textTime.textColour_B), 20 , 10, ALLEGRO_ALIGN_LEFT, "TIME: %i",TEMPO_JOGO_SEGUNDOS - (tempJogo/FPS));
    if(ultimaChance.ativo)al_draw_textf(ultimaChance.objetoFont, al_map_rgb(textTime.textColour_R, textTime.textColour_G, textTime.textColour_B), ultimaChance.posX-250, ultimaChance.posY, ALLEGRO_ALIGN_LEFT, ultChTexto);
    for(i=0;i<QTDE_LINHAS_ALGORITMO;i++){
    al_draw_text(Quadro.objetoFont, al_map_rgb(255, 255, 0), (float)(TELA_LARGURA-Quadro.largura+30), posYTextQuadro, ALLEGRO_ALIGN_LEFT, scriptTXT[i]);
    posYTextQuadro += 60;
    }
}
void telaJogo(){//----------------------------------------------------FUNCAO RESPONSALVEL PELO JOGO
    int i;

    if(inicializacao<200){//---------------------------------------------PAUSA APOS OCORRENCIA DE COLISAO(JOGO INATIVO)
        //------------------------------------------------------------MANTEM BACKGROUND EM MOVIMENTO
        backGround.posX -= carro.vel;
        if(backGround.posX<=(TELA_LARGURA*-2)){
            backGround.posX=0;
        }
        //------------------------------------------------------------PISCA NAVE ENQUANTO AGUARDA RETOMADA DO JOGO
        piscaCarro++;
        if(piscaCarro>=(200/40)){
            if(carro.ativo==false){
                carro.ativo = true;
            }else{
                carro.ativo = false;
            }
            piscaCarro=0;
        }
        inicializacao++;
    }else if(inicializacao==200){//-------------------------------------RETOMADA DO JOGO APOS COLISAO
        inicializacao=4000;
        carro.ativo = true;

    }else{//----------------------------------------------------------ROTINA DO JOGO ATIVO

        //------------------------------------------------------------TRATAMENTO DAS POSIÇÕES E VELOCIDADES(TODOS OS OBJETOS)
        //Posicionamento do background (VELOCIDADE DO CARRO)
        backGround.posX -= carro.vel;

        if(backGround.posX<=(TELA_LARGURA*-2)){
            backGround.posX=0;
        }

        //Posicionamento dos tuneis 1, 2 e 3
        if((tunel1.posX<tunel1.largura*-3)&&(tunel2.posX<tunel2.largura*-3)&&(tunel3.posX<tunel3.largura*-3)){
            //Reposicionamento dos Tuneis
            tunel1.posX = (rand() % tunel1.largura) + TELA_LARGURA + tunel1.largura*2;
            tunel2.posX = (rand() % tunel2.largura) + TELA_LARGURA + tunel2.largura*2;
            tunel3.posX = (rand() % tunel3.largura) + TELA_LARGURA + tunel3.largura*2;

            //Pontuacao:
            if(strcmp(ultChTexto,"ACERTOU!!!")==0){
                strcpy(ultChTexto," ");
                numAcertos++;
                somaScore = (int)((numAcertos*100/numTestes));
            }

            //Detecção de fim de algoritmo:
            if(proxLinha>QTDE_LINHAS_ALGORITMO){
                telaAtual = TELA_RANKING;
            //Avanço de Marcador de linha
            }else {
                proxLinha++;
                seta.posY += 60;
                inverteOrdemTxt(proxLinha);
            }
        }else{
            tunel1.posX -= carro.vel;
            tunel2.posX -= carro.vel;
            tunel3.posX -= carro.vel;
        }



        //------------------------------------------------------------TRATAMENTO DAS COLISÕES
        if (colisao(carro,tunel1)){
            passTunel = 1;
            if(strcmp(txtTunel_1[proxLinha]," ")!=0){
                ultimaChance.ativo = true;
                if(strcmp(tunel1.texto,txtTunel_1[proxLinha])==0){
                    strcpy(ultChTexto,"ACERTOU!!!");
                }else{
                    strcpy(ultChTexto,"ERROU!!!");
                }
            }
        }else if (colisao(carro,tunel2)){
            passTunel = 2;
            if(strcmp(txtTunel_2[proxLinha]," ")!=0){
                ultimaChance.ativo = true;
                if(strcmp(tunel2.texto,txtTunel_1[proxLinha])==0){
                    strcpy(ultChTexto,"ACERTOU!!!");
                }else{
                    strcpy(ultChTexto,"ERROU!!!");
                }
            }
        }else if (colisao(carro,tunel3)){
            passTunel = 3;
            if(strcmp(txtTunel_3[proxLinha]," ")!=0){
                ultimaChance.ativo = true;
                if(strcmp(tunel3.texto,txtTunel_1[proxLinha])==0){
                    strcpy(ultChTexto,"ACERTOU!!!");
                }else{
                    strcpy(ultChTexto,"ERROU!!!");
                }
            }
        }else{
            passTunel = 0;
        }

        //------------------------------------------------------------CONTROLES DE TEMPO
        /*
        //Tempo de duracao do jogo
        if(tempJogo>TEMPO_JOGO_SEGUNDOS*FPS){
            tempTelaFimTempo = 0;
            telaAtual=TELA_FIM_TEMPO;
        }else tempJogo++;
        */
        //Tempo de pisca-Seta(Algoritmo)
        if(seta.temp>FPS/3){
            seta.temp=0;
            if(seta.ativo)seta.ativo=false;
            else seta.ativo=true;
        }else seta.temp++;

        //------------------------------------------------------------LIMITE SUP E INF
        //----------LIMITES DA TELA
        if (carro.posY <= carro.altura/2){
            carro.posY = carro.altura/2;
        }
        if (carro.posY >= TELA_ALTURA-carro.altura/2){
            carro.posY =  TELA_ALTURA-carro.altura/2;
        }
        if (carro.posX <= carro.largura/2){
            carro.posX = carro.largura/2;
        }
        if (carro.posX >= TELA_LARGURA-Quadro.largura-carro.largura/2){
            carro.posX =  TELA_LARGURA-Quadro.largura-carro.largura/2;
        }

        //----------LIMITES DO TUNEL
        switch(passTunel){
        case 1:
            if (carro.posY >= FAIXA1_POS_Y-carro.altura/2){
            carro.posY =  FAIXA1_POS_Y-carro.altura/2;
        }
            break;
        case 2:
            if (carro.posY <= FAIXA1_POS_Y + carro.altura/2){
                carro.posY = FAIXA1_POS_Y + carro.altura/2;
            }else if(carro.posY >= FAIXA2_POS_Y-carro.altura/2){
                carro.posY =  FAIXA2_POS_Y-carro.altura/2;
            }
            break;
        case 3:
            if (carro.posY <= FAIXA2_POS_Y + carro.altura/2){
                carro.posY = FAIXA2_POS_Y + carro.altura/2;
            }
            break;
        }

        //------------------------------------------------------------CONTROLES DO TECLADO
        //Movimentacao lateral
        if(cima){
            carro.posY-= velDesloc;
            //carro.ang=(6.05865);
        }else if(baixo){
            carro.posY+= velDesloc;
            //carro.ang=(0.124533);
        }else{
            carro.ang= 2*ALLEGRO_PI;
        }
        //Direcao
        if(esquerda){
            carro.posX-= velDesloc;
        }else if(direita){
            carro.posX+= velDesloc;
        }
        //Aceleracao
        if(acelera){
            if(carro.vel<VELOCIDADE_MAXIMA) carro.vel = VELOCIDADE_MAXIMA;
            else carro.vel+=carro.vel/150;
        }else if(freia){
            if(carro.vel<VELOCIDADE_MINIMA) carro.vel = VELOCIDADE_MINIMA;
            else carro.vel-=carro.vel/50;
        }else{
            if(carro.vel<VELOCIDADE_MINIMA) carro.vel = VELOCIDADE_MINIMA;
            else carro.vel-=carro.vel/180;
        }

    }
    drawTelaJogo();

}
void telaFimTempo(){
    //Monta a telaFimTempo
     al_draw_bitmap(backGroundFimTempo.objetoBitmap, 0, 0, 0);
     tempTelaFimTempo++;
     if(tempTelaFimTempo>4*FPS){
        telaAtual = TELA_RANKING;
     }
}
void inicializaObjetos(){
    int i;
    backGround.ativo=true;
    backGround.posX=0;
    backGround.posY=0;

    backGroundCidade.ativo=true;
    backGroundMenu.ativo=true;
    backGroundEscolha.ativo=true;
    backGroundRanking.ativo=true;
    backGroundPause.ativo=true;
    backGroundFimTempo.ativo=true;

    carro.ativo=true;
    carro.posX = 200;
    carro.posY= 500;
    carro.xColisao = -90;
    carro.yColisao = -70;
    carro.x1Colisao = -90;
    carro.y1Colisao = -100;
    carro.ang= 2*ALLEGRO_PI;
    carro.vel= 20;

    tunel1.ativo=true;
    tunel1.posX = backGround.posX + TELA_LARGURA;
    tunel1.posY = 1*(TELA_ALTURA/5);
    tunel1.xColisao = 30;
    tunel1.yColisao = 10;
    tunel1.x1Colisao = -30;
    tunel1.y1Colisao = -10;
    tunel1.vel  = 4;

    tunel2.ativo=true;
    tunel2.posX = backGround.posX + TELA_LARGURA;
    tunel2.posY = 2*(TELA_ALTURA/5);
    tunel2.xColisao = 30;
    tunel2.yColisao = 10;
    tunel2.x1Colisao = -30;
    tunel2.y1Colisao = -10;
    tunel2.vel = 4;

    tunel3.ativo=true;
    tunel3.posX = backGround.posX + TELA_LARGURA;
    tunel3.posY = 3*(TELA_ALTURA/5);
    tunel3.xColisao = 30;
    tunel3.yColisao = 10;
    tunel3.x1Colisao = -30;
    tunel3.y1Colisao = -10;
    tunel3.vel = 4;

    Quadro.ativo=true;
    Quadro.posX = TELA_LARGURA - Quadro.largura;
    Quadro.posY = 0;
    Quadro.xColisao = 30;
    Quadro.yColisao = 10;
    Quadro.x1Colisao = -30;
    Quadro.y1Colisao = -10;
    Quadro.vel = 4;

    seta.ativo = true;

    btPlay.ativo=true;
    btPlay.posX = 468;
    btPlay.posY = 520;

    btExit.ativo=true;
    btExit.posX = 1000;
    btExit.posY = 520;

    btPlayAgain.ativo=true;
    btPlayAgain.posX = 807;
    btPlayAgain.posY = 110;

    btQuit.ativo=true;
    btQuit.posX = 1430;
    btQuit.posY = 110;

    btCarro1.ativo=true;
    btCarro1.posX = 303;
    btCarro1.posY = 85;

    btCarro2.ativo=true;
    btCarro2.posX = 303;
    btCarro2.posY = 390;

    btCarro3.ativo=true;
    btCarro3.posX = 303;
    btCarro3.posY = 690;

    btCenarioEspaco.ativo=true;
    btCenarioEspaco.posX = 1503;
    btCenarioEspaco.posY = 160;

    btCenarioCidade.ativo=true;
    btCenarioCidade.posX = 1515;
    btCenarioCidade.posY = 620;

    btPlayJogo.ativo=true;
    btPlayJogo.posX = 840;
    btPlayJogo.posY = 738;

    btPlayPause.ativo=true;
    btPlayPause.posX = 248;
    btPlayPause.posY = 283;

    btExitPause.ativo=true;
    btExitPause.posX = 1462;
    btExitPause.posY = 280;

    textTime.ativo=true;
    textTime.posX = 20;
    textTime.posY = 20;
    textTime.textColour_R = 0;
    textTime.textColour_G = 255;
    textTime.textColour_B = 255;

    textRank.ativo=true;

    textScoreJogo.ativo=true;

    textPlayer.ativo=true;

    ultimaChance.ativo=true;
    ultimaChance.posX = 1300;
    ultimaChance.posY = 20;
}
