#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

//Parametros gerais
#define FPS             60.0
#define LARGURA_TELA    1000
#define ALTURA_TELA     1000
#define LARGURA_QUADRO  330
#define ALTURA_QUADRO   1000
//Comandos (Instrucoes disponiveis):
#define RUN             0
#define JUMP            1
#define IDLE            2
#define DEAD            3
#define SLIDE           4
#define PEGA            5
#define ABRE            6
#define CONDICIONAL_SE  7
#define REPETICAO       8
//Parametros para instrucoes:
#define LEFT            0
#define RIGHT           1
#define UP              2
#define DOWN            3
#define DESLOCAMENTO    91 // = TELA(1000) / NUMERO DE QUADRADOS (11)
#define QTDE_LIN_ALGO   23
#define TAM_LIN_ALGO    23
#define ESPACAMENTO_LIN 15

#define FLOAT_TO_INT(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_DISPLAY *quadro =NULL;
ALLEGRO_DISPLAY *teste =NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;

typedef struct{ //Objetos
    //Caraceristicas do Objeto
    ALLEGRO_BITMAP *img;
    ALLEGRO_BITMAP *imgInverso;
    int posicaoX;
    int posicaoY;
    int contador;
    int velocidade;
    int ativo;
    int inverso;
    //Fonte(Texto) do Objeto
    ALLEGRO_FONT *font;
    int posicaoFontX;
    int posicaoFontY;
    int fontAtivo;
    char texto[200];
    int textColour_R;
    int textColour_G;
    int textColour_B;
    int tamanhoFont;
}Objeto;

typedef struct{ //Personagens
    ALLEGRO_BITMAP *spriteImg[5][10];
    int acao;
    int spriteAtual;
    int contSprite;
    float velocidadeSprite;
    int frames_spriteRun;
    int frames_spriteJump;
    int frames_spriteDead;
    int frames_spriteIdle;
    int frames_spriteSlide;
    int posicaoX;
    int posicaoY;
    int velocidadeX;
    int velocidadeY;
    int sentido;
    int contDesloc;
    int contBlocos;
    int ativo;
}Personagem;

typedef struct{ //InputDevice
    int enter;
    int espaco;
    int tab;
    int backSpace;
    int caracterPendente;
    int teclaPress;
    int cont;
    char cursor;
    int cursorAtivo;
    char codASCII;
    int codTecla;
    int posicaoX;
    int posicaoY;
    int click;
    int ajusteX;
    int ajusteY;
}InputDevice;

typedef struct{//InstrucaoPadrao
    Personagem *P;
    int comando;//nome da instrucao
    int direcao;
    int iteracao;
    int acao;//escorregar, correa, pular etc...
    Objeto *objeto;
    int contIteracao; //Utilizado para controle das REPETICOES
    int qtdeIteracao; //Utilizado para controle das REPETICOES
    int qtdeComandos; //Utilizado para REPETICOES e CONDICIONAIS
}InstrucaoPadrao;

/* PERSONAGENS, OBJETOS, INPUT DEVICES E VETOR DE INSTRUCAO ------------------------------------*/
Objeto backgroundJogo;
Objeto backgroundQuadro;
Objeto marcador;
Objeto btnCompilar; //Fonte utilizada para numerar as linas do quadro
Objeto btnLimpar;
Objeto chave;
Objeto banana;
Objeto casa2;
Personagem cat;
InputDevice teclado;
InputDevice mouse;
InstrucaoPadrao vetorInstrucao[30];

/* VARIAVEIS GLOBAIS ------------------------------------*/
//Variaveis para processamento de instrucoes:
int Linha = 0; // Indice do vetor de Instrucoes
int nInstrucao = 0; //Qtde de instrucoes
int LinhaRepeticao=QTDE_LIN_ALGO+1; //armazena a localizacao da repeticao ativa
int LinhaFimRepeticao=0; //armazena a localizacao do fim repeticao ativa
int repeticaoAtiva=0; //Informa se repeticao ja foi satisfeita ou nao
int LinhaCondicional=QTDE_LIN_ALGO+1; //armazena a localizacao da Condicional ativa
int LinhaFimCondicional=0; //armazena a localizacao do fim Condicional ativa
int condicionalAtiva=0; //Informa se Condicional ja foi satisfeita ou nao

//texto do Quadro Negro
char txtAlgoritmo[QTDE_LIN_ALGO][TAM_LIN_ALGO+1];
int linhaAlgoritmo=0, colunaAlgoritmo=0; //Indices do texto

void error_msg(char *text){
	al_show_native_message_box(janela,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int initObjetos(){
    //carrega o backgroundJogo
    backgroundJogo.img = al_load_bitmap("img/background.jpg");
    if (!backgroundJogo.img){
        return 0;
    }
    backgroundJogo.ativo=1;
    backgroundJogo.posicaoX=0;
    backgroundJogo.posicaoY=0;

    //carrega o backgroundJogo
    backgroundQuadro.img = al_load_bitmap("img/quadro-negro.jpg");
    if (!backgroundQuadro.img){
        return 0;
    }
    backgroundQuadro.ativo=1;
    backgroundQuadro.posicaoX=0;
    backgroundQuadro.posicaoY=0;
    //Fonte do quadro
    backgroundQuadro.tamanhoFont = 20;
    backgroundQuadro.fontAtivo = 1;
    backgroundQuadro.posicaoFontX = 25;
    backgroundQuadro.posicaoFontY = 100;
    backgroundQuadro.textColour_R = 255;
    backgroundQuadro.textColour_G = 255;
    backgroundQuadro.textColour_B = 255;
    strcpy(backgroundQuadro.texto,"COMANDOS AQUI:");
    backgroundQuadro.font = al_load_font("fonte/alarm_clock/alarm_clock.ttf", backgroundQuadro.tamanhoFont, ALLEGRO_TTF_NO_KERNING);
    if (!backgroundQuadro.font){
        return 0;
    }

    //carrega marcador
    marcador.ativo = 0;
    marcador.posicaoX = 0;
    marcador.posicaoY = backgroundQuadro.posicaoFontY;
    marcador.velocidade = 30;
    marcador.contador = 0; //Utilizado para contar qtde de Chaves fechadas (p/ Alinhamento)

    //carrega o botao Compilar
    btnCompilar.img = al_load_bitmap("img/btnCompilar.png");
    if (!btnCompilar.img){
        return 0;
    }
    btnCompilar.imgInverso = al_load_bitmap("img/btnCompilarInverso.png");
    if (!btnCompilar.imgInverso){
        return 0;
    }
    btnCompilar.ativo=1;
    btnCompilar.posicaoX= (LARGURA_QUADRO/3)-50;
    btnCompilar.posicaoY= ALTURA_QUADRO-al_get_bitmap_height(btnCompilar.img)-10;
    //Fonte da numarecao de linhas do quadro
    btnCompilar.tamanhoFont = 15;
    btnCompilar.fontAtivo = 1;
    btnCompilar.posicaoFontX = 25;
    btnCompilar.posicaoFontY = 100;
    btnCompilar.textColour_R = 255;   //Amarelo
    btnCompilar.textColour_G = 255;
    btnCompilar.textColour_B = 0;
    strcpy(btnCompilar.texto,"COMANDOS AQUI:");
    btnCompilar.font = al_load_font("fonte/alarm_clock/alarm_clock.ttf", btnCompilar.tamanhoFont, ALLEGRO_TTF_NO_KERNING);
    if (!btnCompilar.font){
        return 0;
    }

    //carrega o botao Limpar
    btnLimpar.img = al_load_bitmap("img/btnLimpar.png");
    if (!btnLimpar.img){
        return 0;
    }
    btnLimpar.imgInverso = al_load_bitmap("img/btnLimparInverso.png");
    if (!btnLimpar.imgInverso){
        return 0;
    }
    btnLimpar.ativo=1;
    btnLimpar.posicaoX= (LARGURA_QUADRO/3)*2;
    btnLimpar.posicaoY= ALTURA_QUADRO-al_get_bitmap_height(btnLimpar.img)-10;

    //CHAVE
    chave.img = al_load_bitmap("img/itens/key.png");
    if (!chave.img){
        return 0;
    }
    chave.ativo=1;
    chave.posicaoX=8*DESLOCAMENTO;
    chave.posicaoY=8*DESLOCAMENTO;
    strcpy(chave.texto, "CHAVE");

    //BANANA
    banana.img = al_load_bitmap("img/itens/cascaBanana.png");
    if (!banana.img){
        return 0;
    }
    banana.ativo=1;
    banana.posicaoX=3*DESLOCAMENTO;
    banana.posicaoY=2*DESLOCAMENTO;
    strcpy(banana.texto, "BANANA");

    //CASA2
    casa2.posicaoX=6*DESLOCAMENTO;
    casa2.posicaoY=8*DESLOCAMENTO;
    //Fonte da casa2
    casa2.tamanhoFont = 30;
    casa2.fontAtivo = 1;
    casa2.posicaoFontX = 4*DESLOCAMENTO;
    casa2.posicaoFontY = 9*DESLOCAMENTO+16;
    casa2.textColour_R = 255;
    casa2.textColour_G = 255;
    casa2.textColour_B = 255;
    strcpy(casa2.texto,"TRANCADA");
    casa2.font = al_load_font("fonte/alarm_clock/alarm_clock.ttf", casa2.tamanhoFont, ALLEGRO_TTF_NO_KERNING);
    if (!casa2.font){
        return 0;
    }

    //Inicializacao teclado
    teclado.backSpace=0;
    teclado.caracterPendente=0;
    teclado.codASCII='\0';
    teclado.enter=0;
    teclado.espaco=0;
    teclado.tab=0;
    teclado.teclaPress=0;
    teclado.cont=0;
    teclado.cursor='_';

    //Inicializacao mouse
    mouse.posicaoX = 0;
    mouse.posicaoY = 0;
    mouse.click = 0;
    mouse.ajusteX = LARGURA_QUADRO;
    mouse.ajusteY = ALTURA_QUADRO;

    return 1;
}

int initPersonagens(){
    int i;

        //posicao X Y da janela em que sera mostrado o sprite
    cat.acao=RUN;
    //COL1=60(X) COL2=390(X)  LIN1=35(Y) LIN2=(Y)
    cat.posicaoX=2*DESLOCAMENTO;
    cat.posicaoY=2*DESLOCAMENTO;
    cat.contSprite=0;
    cat.spriteAtual=0;
    cat.velocidadeX=1;
    cat.velocidadeY=1;
    cat.velocidadeSprite=59;
    cat.ativo=1;
    cat.contDesloc=0;
    cat.contBlocos=0;

    //carrega a folha de sprites na variavel (IDLE - CAT)
    cat.frames_spriteIdle = 10;
    cat.spriteImg[IDLE][0] = al_load_bitmap("img/cat/Idle (1).png");
    cat.spriteImg[IDLE][1] = al_load_bitmap("img/cat/Idle (2).png");
    cat.spriteImg[IDLE][2] = al_load_bitmap("img/cat/Idle (3).png");
    cat.spriteImg[IDLE][3] = al_load_bitmap("img/cat/Idle (4).png");
    cat.spriteImg[IDLE][4] = al_load_bitmap("img/cat/Idle (5).png");
    cat.spriteImg[IDLE][5] = al_load_bitmap("img/cat/Idle (6).png");
    cat.spriteImg[IDLE][6] = al_load_bitmap("img/cat/Idle (7).png");
    cat.spriteImg[IDLE][7] = al_load_bitmap("img/cat/Idle (8).png");
    cat.spriteImg[IDLE][8] = al_load_bitmap("img/cat/Idle (9).png");
    cat.spriteImg[IDLE][9] = al_load_bitmap("img/cat/Idle (10).png");
    for(i=0;i<cat.frames_spriteIdle;i++)
    {
        if (!cat.spriteImg[IDLE][i]){
            return 0;
        }
    }

    //carrega a folha de sprites na variavel (RUN - CAT)
    cat.frames_spriteRun = 10;
    cat.spriteImg[RUN][0] = al_load_bitmap("img/cat/Run (1).png");
    cat.spriteImg[RUN][1] = al_load_bitmap("img/cat/Run (1).png");
    cat.spriteImg[RUN][2] = al_load_bitmap("img/cat/Run (2).png");
    cat.spriteImg[RUN][3] = al_load_bitmap("img/cat/Run (3).png");
    cat.spriteImg[RUN][4] = al_load_bitmap("img/cat/Run (4).png");
    cat.spriteImg[RUN][5] = al_load_bitmap("img/cat/Run (5).png");
    cat.spriteImg[RUN][6] = al_load_bitmap("img/cat/Run (6).png");
    cat.spriteImg[RUN][7] = al_load_bitmap("img/cat/Run (6).png");
    cat.spriteImg[RUN][8] = al_load_bitmap("img/cat/Run (7).png");
    cat.spriteImg[RUN][9] = al_load_bitmap("img/cat/Run (8).png");
    for(i=0;i<cat.frames_spriteRun;i++)
    {
        if (!cat.spriteImg[RUN][i]){
            return 0;
        }
    }

    //carrega a folha de sprites na variavel (JUMP - CAT)
    cat.frames_spriteJump = 10;
    cat.spriteImg[JUMP][0] = al_load_bitmap("img/cat/Jump (3).png");
    cat.spriteImg[JUMP][1] = al_load_bitmap("img/cat/Jump (4).png");
    cat.spriteImg[JUMP][2] = al_load_bitmap("img/cat/Jump (5).png");
    cat.spriteImg[JUMP][3] = al_load_bitmap("img/cat/Jump (6).png");
    cat.spriteImg[JUMP][4] = al_load_bitmap("img/cat/Jump (7).png");
    cat.spriteImg[JUMP][5] = al_load_bitmap("img/cat/Jump (8).png");
    cat.spriteImg[JUMP][6] = al_load_bitmap("img/cat/Jump (3).png");
    cat.spriteImg[JUMP][7] = al_load_bitmap("img/cat/Jump (4).png");
    cat.spriteImg[JUMP][8] = al_load_bitmap("img/cat/Jump (5).png");
    cat.spriteImg[JUMP][9] = al_load_bitmap("img/cat/Jump (6).png");
    for(i=0;i<cat.frames_spriteJump;i++)
    {
        if (!cat.spriteImg[JUMP][i]){
            return 0;
        }
    }

    //carrega a folha de sprites na variavel (DEAD - CAT)
    cat.frames_spriteDead = 10;
    cat.spriteImg[DEAD][0] = al_load_bitmap("img/cat/Dead (1).png");
    cat.spriteImg[DEAD][1] = al_load_bitmap("img/cat/Dead (2).png");
    cat.spriteImg[DEAD][2] = al_load_bitmap("img/cat/Dead (3).png");
    cat.spriteImg[DEAD][3] = al_load_bitmap("img/cat/Dead (4).png");
    cat.spriteImg[DEAD][4] = al_load_bitmap("img/cat/Dead (5).png");
    cat.spriteImg[DEAD][5] = al_load_bitmap("img/cat/Dead (6).png");
    cat.spriteImg[DEAD][6] = al_load_bitmap("img/cat/Dead (7).png");
    cat.spriteImg[DEAD][7] = al_load_bitmap("img/cat/Dead (8).png");
    cat.spriteImg[DEAD][8] = al_load_bitmap("img/cat/Dead (9).png");
    cat.spriteImg[DEAD][9] = al_load_bitmap("img/cat/Dead (10).png");
    for(i=0;i<cat.frames_spriteDead;i++)
    {
        if (!cat.spriteImg[DEAD][i]){
            return 0;
        }
    }

    //carrega a folha de sprites na variavel (SLIDE - CAT)
    cat.frames_spriteSlide = 10;
    cat.spriteImg[SLIDE][0] = al_load_bitmap("img/cat/Slide (1).png");
    cat.spriteImg[SLIDE][1] = al_load_bitmap("img/cat/Slide (2).png");
    cat.spriteImg[SLIDE][2] = al_load_bitmap("img/cat/Slide (3).png");
    cat.spriteImg[SLIDE][3] = al_load_bitmap("img/cat/Slide (4).png");
    cat.spriteImg[SLIDE][4] = al_load_bitmap("img/cat/Slide (5).png");
    cat.spriteImg[SLIDE][5] = al_load_bitmap("img/cat/Slide (6).png");
    cat.spriteImg[SLIDE][6] = al_load_bitmap("img/cat/Slide (7).png");
    cat.spriteImg[SLIDE][7] = al_load_bitmap("img/cat/Slide (8).png");
    cat.spriteImg[SLIDE][8] = al_load_bitmap("img/cat/Slide (9).png");
    cat.spriteImg[SLIDE][9] = al_load_bitmap("img/cat/Slide (10).png");
    for(i=0;i<cat.frames_spriteSlide;i++)
    {
        if (!cat.spriteImg[SLIDE][i]){
            return 0;
        }
    }

    return 1;
}

int inicializar(){
    if (!al_init()){
        error_msg("Falha ao inicializar a Allegro");
        return 0;
    }
    if (!al_init_image_addon()){
        error_msg("Falha ao inicializar o addon de imagens");
        return 0;
    }

    //Funcao nao retorna valor (void)
    al_init_font_addon();

    if (!al_init_ttf_addon()){
        error_msg("Falha ao inicializar o addon de ttf");
        return 0;
    }

    if (!al_init_primitives_addon())
    {
        error_msg("Falha ao inicializar add-on de primitivas.\n");
        return 0;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        error_msg("Falha ao criar temporizador");
        return 0;
    }

    ALLEGRO_MONITOR_INFO monitor;
    al_get_monitor_info(0, &monitor);
    al_set_new_display_flags(ALLEGRO_RESIZABLE);


    quadro = al_create_display(LARGURA_QUADRO,ALTURA_QUADRO);
    if(!quadro) {
        error_msg("Falha ao criar quadro");
        return 0;
    }
    al_set_window_title(quadro, "Compilador");
    al_set_window_position(quadro,LARGURA_TELA,monitor.y1);

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela) {
        error_msg("Falha ao criar janela");
        return 0;
    }
    al_set_window_title(janela, "Projeto Wash - CTI Renato Archer");
    al_set_window_position(janela,monitor.x1,monitor.y1);


    //inicializa addon do teclado
    if(!al_install_keyboard()){
        error_msg("Falha ao inicializar teclado");
        return 0;
    }

    //inicializa addon do mouse
    if(!al_install_mouse()){
        error_msg("Falha ao inicializar mouse");
        return 0;
    }

    // Atribui o cursor padr�o do sistema para ser usado
    al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    al_set_system_mouse_cursor(quadro, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);


    fila_eventos = al_create_event_queue();
    if(!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        return 0;
    }

    if(!initPersonagens()){
        error_msg("Falha ao carregar Personagens");
        return 0;
    }


    if(!initObjetos()){
        error_msg("Falha ao carregar Objetos");
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(quadro));
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_start_timer(timer);

    return 1;
}

int drawTelaJogo(Personagem *P){
    int i;
    //Coloca tela do jogo (janela) como alvo  **********************************************
    al_set_target_bitmap(al_get_backbuffer(janela));
    //desenha o backgroundJogo na tela
    if(backgroundJogo.ativo)al_draw_scaled_bitmap(backgroundJogo.img,0.0,0.0, al_get_bitmap_height(backgroundJogo.img), al_get_bitmap_width(backgroundJogo.img), 0.0,0.0, (float)al_get_display_height(janela), (float)al_get_display_width(janela),0);
    if(casa2.fontAtivo) al_draw_textf(casa2.font,
                          al_map_rgb(casa2.textColour_R,casa2.textColour_G,casa2.textColour_B),
                          casa2.posicaoFontX,
                          casa2.posicaoFontY,
                          0,
                          casa2.texto);
    if(chave.ativo)al_draw_bitmap(chave.img,chave.posicaoX,chave.posicaoY,0);
    if(banana.ativo)al_draw_bitmap(banana.img,banana.posicaoX,banana.posicaoY,0);
    //desenha sprite na posicao X Y da janela, a partir da regiao X Y da folha
    if(P->ativo)al_draw_bitmap(P->spriteImg[P->acao][P->spriteAtual], P->posicaoX+(DESLOCAMENTO/2)-(al_get_bitmap_width(P->spriteImg[P->acao][P->spriteAtual])/2), P->posicaoY-(al_get_bitmap_height(P->spriteImg[P->acao][P->spriteAtual])-DESLOCAMENTO), P->sentido);

    al_flip_display();



    //Coloca tela do compilador (quadro) como alvo **********************************************
    al_set_target_bitmap(al_get_backbuffer(quadro));
    //al_clear_to_color(al_map_rgb(20,100,20));
    if(backgroundQuadro.ativo)al_draw_scaled_bitmap(backgroundQuadro.img,0.0,0.0,al_get_bitmap_width(backgroundQuadro.img), al_get_bitmap_height(backgroundQuadro.img), 0.0,0.0, (float)al_get_display_width(quadro), (float)al_get_display_height(quadro),0);
    if(backgroundQuadro.fontAtivo){
        for(i=0;i<QTDE_LIN_ALGO;i++){
            al_draw_textf(btnCompilar.font,
                          al_map_rgb(btnCompilar.textColour_R,btnCompilar.textColour_G,btnCompilar.textColour_B),
                          5,
                          (backgroundQuadro.posicaoFontY)+(backgroundQuadro.tamanhoFont*i)+(ESPACAMENTO_LIN*i),
                          0,
                          "%i",i);
        }
        for(i=0;i<QTDE_LIN_ALGO;i++){
            al_draw_textf(backgroundQuadro.font,
                          al_map_rgb(backgroundQuadro.textColour_R,backgroundQuadro.textColour_G,backgroundQuadro.textColour_B),
                          backgroundQuadro.posicaoFontX,
                          (backgroundQuadro.posicaoFontY)+(backgroundQuadro.tamanhoFont*i)+(ESPACAMENTO_LIN*i),
                          0,
                          &txtAlgoritmo[i][0]);
        }
    }
    if((btnCompilar.ativo)&&(btnCompilar.inverso))al_draw_bitmap(btnCompilar.imgInverso,btnCompilar.posicaoX,btnCompilar.posicaoY,0);
    else if(btnCompilar.ativo)al_draw_bitmap(btnCompilar.img,btnCompilar.posicaoX,btnCompilar.posicaoY,0);
    if((btnLimpar.ativo)&&(btnLimpar.inverso))al_draw_bitmap(btnLimpar.imgInverso,btnLimpar.posicaoX,btnLimpar.posicaoY,0);
    else if(btnLimpar.ativo)al_draw_bitmap(btnLimpar.img,btnLimpar.posicaoX,btnLimpar.posicaoY,0);
    //marcador
    if(marcador.ativo)al_draw_rectangle(marcador.posicaoX,marcador.posicaoY-10,marcador.posicaoX+al_get_display_width(quadro),marcador.posicaoY+backgroundQuadro.tamanhoFont+10,al_map_rgb(20, 255, 20),6);

    al_flip_display();

    return 0;
}

void runPersonagem(Personagem *P, int Direcao){
    //Seta acao atual a ser desenhada na tela
    P->acao = RUN;

    //Condicional para Troca de sprite velocidade em funcao da velocidade setada
    if( P->contSprite > (FPS - P->velocidadeSprite) ){
        //Vai para proximo sprite
        P->spriteAtual++;
        //Se chegou ao ultimo sprite              Retorna para o primeiro
        if(P->spriteAtual>=P->frames_spriteRun-1) P->spriteAtual=0;
        //Reseta variavel para controle de velocidade da troca de sprite
        P->contSprite=0;
        //Incrementa variavel para controle de velocidade da troca de sprite
    }else P->contSprite++;

    //deslocamento vertical/horizontal
    switch(Direcao){
    case LEFT:
        P->posicaoX-= P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = ALLEGRO_FLIP_HORIZONTAL;
        break;
    case RIGHT:
        P->posicaoX+=P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = 0;
        break;
    case UP:
        P->posicaoY-=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        P->sentido = 0;
        break;
    case DOWN:
        P->posicaoY+=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        break;
    }

    //Controle de Deslocamento por Blocos
    if(P->contDesloc>=DESLOCAMENTO){
        P->contBlocos++;
        P->contDesloc=0;
    }
}

void idlePersonagem(Personagem *P, int Direcao){
    //SPRITES
    P->acao = IDLE;
    if( P->contSprite > (FPS - P->velocidadeSprite) ){
        P->spriteAtual++;
        if(P->spriteAtual>=P->frames_spriteIdle-1) P->spriteAtual=0;
        P->contSprite=0;
    }else P->contSprite++;
    //DESLOCAMENTO
    switch(Direcao){
    case LEFT:
        P->contDesloc+= P->velocidadeX;
        P->sentido = ALLEGRO_FLIP_HORIZONTAL;
        break;
    case RIGHT:
        P->contDesloc+= P->velocidadeX;
        P->sentido = 0;
        break;
    case UP:
        P->contDesloc+= P->velocidadeY;
        P->sentido = 0;
        break;
    case DOWN:
        P->contDesloc+= P->velocidadeY;
        break;
    }
    //BLOCOS
    if(P->contDesloc>=DESLOCAMENTO){
        P->contBlocos++;
        P->contDesloc=0;
    }
}

void jumpPersonagem(Personagem *P, int Direcao){
    int aux;
        //SPRITES
    P->acao = JUMP;
    if( P->contSprite > (FPS - P->velocidadeSprite) ){
        P->spriteAtual++;
        if(P->spriteAtual>=P->frames_spriteJump-1) P->spriteAtual=0;
        P->contSprite=0;
    }else P->contSprite++;
    //DESLOCAMENTO
    switch(Direcao){
    case LEFT:
        P->posicaoX-= P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = ALLEGRO_FLIP_HORIZONTAL;
        break;
    case RIGHT:
        P->posicaoX+=P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = 0;
        break;
    case UP:
        P->posicaoY-=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        P->sentido = 0;
        break;
    case DOWN:
        P->posicaoY+=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        break;
    }
    //BLOCOS
    if(P->contDesloc>=DESLOCAMENTO){
        P->contBlocos++;
        P->contDesloc=0;
        //Alinhamento do personagem no x,y dos blocos
        aux = FLOAT_TO_INT((float)(P->posicaoX)/(float)(DESLOCAMENTO));
        P->posicaoX = aux * DESLOCAMENTO;
        aux = FLOAT_TO_INT((float)(P->posicaoY)/(float)(DESLOCAMENTO));
        P->posicaoY = aux * DESLOCAMENTO;
    }
}

void deadPersonagem(Personagem *P, int Direcao){
        //SPRITES
    P->acao = DEAD;
    if( P->contSprite > (FPS - P->velocidadeSprite) ){
        P->spriteAtual++;
        if(P->spriteAtual>=P->frames_spriteDead-1) P->spriteAtual=0;
        P->contSprite=0;
    }else P->contSprite++;
    //DESLOCAMENTO
    switch(Direcao){
    case LEFT:
        P->posicaoX-= P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = ALLEGRO_FLIP_HORIZONTAL;
        break;
    case RIGHT:
        P->posicaoX+=P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = 0;
        break;
    case UP:
        P->posicaoY-=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        P->sentido = 0;
        break;
    case DOWN:
        P->posicaoY+=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        break;
    }
    //BLOCOS
    if(P->contDesloc>=DESLOCAMENTO){
        P->contBlocos++;
        P->contDesloc=0;
    }
}

void slidePersonagem(Personagem *P, int Direcao){
        //SPRITES
    P->acao = SLIDE;
    if( P->contSprite > (FPS - P->velocidadeSprite) ){
        P->spriteAtual++;
        if(P->spriteAtual>=P->frames_spriteSlide-1) P->spriteAtual=0;
        P->contSprite=0;
    }else P->contSprite++;
    //DESLOCAMENTO
    switch(Direcao){
    case LEFT:
        P->posicaoX-= P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = ALLEGRO_FLIP_HORIZONTAL;
        break;
    case RIGHT:
        P->posicaoX+=P->velocidadeX;
        P->contDesloc+= P->velocidadeX;
        P->sentido = 0;
        break;
    case UP:
        P->posicaoY-=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        P->sentido = 0;
        break;
    case DOWN:
        P->posicaoY+=P->velocidadeY;
        P->contDesloc+= P->velocidadeY;
        break;
    }
    //BLOCOS
    if(P->contDesloc>=DESLOCAMENTO){
        P->contBlocos++;
        P->contDesloc=0;
    }
}

int acao(Personagem *P, int movimento, int iteracao, int Direcao){
    int fimAcao=0,aux;
    if(P->contBlocos>=iteracao){
        P->contBlocos=0;
        P->contDesloc=0;
        if((P->acao==RUN)||(P->acao==JUMP)){
                P->acao = RUN;
                P->spriteAtual= 0;
        }
        //Alinhamento do personagem no x,y dos blocos
        aux = FLOAT_TO_INT((float)(P->posicaoX)/(float)(DESLOCAMENTO));
        P->posicaoX = aux * DESLOCAMENTO;
        aux = FLOAT_TO_INT((float)(P->posicaoY)/(float)(DESLOCAMENTO));
        P->posicaoY = aux * DESLOCAMENTO;

        fimAcao = 1;
    }else{
        switch(movimento){
        case RUN:
            runPersonagem(P, Direcao);
            break;
        case JUMP:
            jumpPersonagem(P, Direcao);
            break;
        case IDLE:
            idlePersonagem(P, Direcao);
            break;
        case DEAD:
            deadPersonagem(P, Direcao);
            break;
        case SLIDE:
            slidePersonagem(P, Direcao);
            break;
        default:
            break;
        }
        fimAcao = 0;
    }
    return fimAcao;
}

void finalizar(){
    int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<10;j++){
            al_destroy_bitmap(cat.spriteImg[i][j]);
        }
    }
    al_destroy_bitmap(backgroundJogo.img);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_display(quadro);
    al_destroy_event_queue(fila_eventos);
}

void tratamentoTeclado(){

    if(teclado.cont>10){//----------------------Tratamento do cursor (Piscante)
        teclado.cont=0;
        if(teclado.cursorAtivo){
            txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo] = teclado.cursor;
            teclado.cursorAtivo=0;
        }else {
            teclado.cursor = txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo];
            txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]= '_';
            teclado.cursorAtivo=1;
        }
    }else teclado.cont++;

    if(teclado.teclaPress){//---------------Se alguma tecla foi pressionada
        if(teclado.caracterPendente){//----------Se foi caracter
            //Escreve caractere
            txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo] = teclado.codASCII;
            txtAlgoritmo[linhaAlgoritmo][TAM_LIN_ALGO] = '\0';
            //Incrementa posicao para proxima escrita
            colunaAlgoritmo++;
            if(colunaAlgoritmo>=TAM_LIN_ALGO){
                colunaAlgoritmo=0;
                linhaAlgoritmo++;
                if(linhaAlgoritmo>=QTDE_LIN_ALGO){
                    linhaAlgoritmo=QTDE_LIN_ALGO-1;
                    colunaAlgoritmo=TAM_LIN_ALGO;
                }
            }

            teclado.caracterPendente=0;
        }else{//---------------------------------Se foi tecla de comando
            if(teclado.codASCII==8){ //BAKSPACE
                teclado.cursorAtivo=0;
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]=teclado.cursor;
                colunaAlgoritmo--;
                if(colunaAlgoritmo<0){
                    txtAlgoritmo[linhaAlgoritmo][TAM_LIN_ALGO-1]='\0';
                    linhaAlgoritmo--;
                    txtAlgoritmo[linhaAlgoritmo][TAM_LIN_ALGO-1]='\0';
                    colunaAlgoritmo=strlen(txtAlgoritmo[linhaAlgoritmo]);
                    if(linhaAlgoritmo<0){
                        linhaAlgoritmo=0;
                        colunaAlgoritmo=0;
                    }
                }
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]= ' '; //apaga letra na posicao atual
                teclado.cursor = txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo];
            }else if(teclado.codASCII==13){//ENTER
                teclado.cursorAtivo=0;
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]=teclado.cursor;
                if(linhaAlgoritmo<QTDE_LIN_ALGO){
                    colunaAlgoritmo=0;
                    linhaAlgoritmo++;
                }
            }else if(teclado.codTecla==ALLEGRO_KEY_UP){//CIMA
                teclado.cursorAtivo=0;
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]=teclado.cursor;
                if(linhaAlgoritmo<=0)linhaAlgoritmo=0;
                else{
                    linhaAlgoritmo--;
                    if(colunaAlgoritmo>strlen(txtAlgoritmo[linhaAlgoritmo]))colunaAlgoritmo=strlen(txtAlgoritmo[linhaAlgoritmo]);
                }
                teclado.cursor = txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo];
                //colunaAlgoritmo=strlen(txtAlgoritmo[linhaAlgoritmo]);
            }else if(teclado.codTecla==ALLEGRO_KEY_DOWN){//BAIXO
                teclado.cursorAtivo=0;
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]=teclado.cursor;
                if(linhaAlgoritmo>=QTDE_LIN_ALGO)linhaAlgoritmo=QTDE_LIN_ALGO-1;
                else {
                    linhaAlgoritmo++;
                    if(colunaAlgoritmo>strlen(txtAlgoritmo[linhaAlgoritmo]))colunaAlgoritmo=strlen(txtAlgoritmo[linhaAlgoritmo]);
                }
                teclado.cursor = txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo];
                //colunaAlgoritmo=strlen(txtAlgoritmo[linhaAlgoritmo]);
            }else if(teclado.codTecla==ALLEGRO_KEY_RIGHT){//DIREITA
                //PARA IMPLEMENTAR ESTA FUNCAO DEVE-SE TAMBEM MODIFICAR ENTER, BACKSPACE E INSERCAO DE NOVOS CARACTERES
                teclado.cursorAtivo=0;
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]=teclado.cursor;
                if(colunaAlgoritmo<strlen(txtAlgoritmo[linhaAlgoritmo]))colunaAlgoritmo++;
                teclado.cursor = txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo];
            }else if(teclado.codTecla==ALLEGRO_KEY_LEFT){//ESQUERDA
                //PARA IMPLEMENTAR ESTA FUNCAO DEVE-SE TAMBEM MODIFICAR ENTER, BACKSPACE E INSERCAO DE NOVOS CARACTERES
                teclado.cursorAtivo=0;
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]=teclado.cursor;
                if(colunaAlgoritmo>0)colunaAlgoritmo--;
                teclado.cursor = txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo];
            }
        }

        teclado.teclaPress=0;
    }
}

void ajustaPosMouse(int mouseX, int mouseY){
    float difX = (float)mouse.ajusteX/(float)LARGURA_QUADRO;
    float difY = (float)mouse.ajusteY/(float)ALTURA_QUADRO;
    mouse.posicaoX = (int)((float)mouseX/difX);
    mouse.posicaoY = (int)((float)mouseY/difY);
}

int mousePosicao(Objeto botao){
    int posicao=0;
    if((mouse.posicaoX>=botao.posicaoX)&&(mouse.posicaoX<=(botao.posicaoX + al_get_bitmap_width(botao.img)))){
        if((mouse.posicaoY>=botao.posicaoY)&&(mouse.posicaoY<=(botao.posicaoY + al_get_bitmap_height(botao.img)))){
            posicao=1;
        }else{
            posicao=0;
        }
    }else{
        posicao=0;
    }
    return posicao;
}

int mouseClick(Objeto botao){
    int click=0;
    if(mouse.click){
        if((mouse.posicaoX>=botao.posicaoX)&&(mouse.posicaoX<=(botao.posicaoX + al_get_bitmap_width(botao.img)))){
            if((mouse.posicaoY>=botao.posicaoY)&&(mouse.posicaoY<=(botao.posicaoY + al_get_bitmap_height(botao.img)))){
                click=1;
                mouse.click=0;
            }else{
                click=0;
            }
        }else{
            click=0;
        }
    }
    return click;
}

int compilador(){
    int i, linhaErro=0, inicio=255;
    int ii,ok,numChavesAbertas=0,numChavesFechadas=0,possui_SE=0;
    char aux[TAM_LIN_ALGO];

    //Percorre algoritmo
    Linha=0;
    nInstrucao=0;
    txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo] = '\0';

    //Procura INICIO do codigo
    for(i=0;i<QTDE_LIN_ALGO;i++){
        if(strncmp(txtAlgoritmo[i],"INICIO",6)==0){
            inicio=i+1;
            break;
        }
    }
    //Compila o algoritmo (Se inicio foi encontrado)
    for(i=inicio;i<QTDE_LIN_ALGO;i++){
        //Finaliza busca e retorna ok caso encontrar FIM do codigo
        if(strncmp(txtAlgoritmo[i],"FIM",3)==0){
            linhaErro=255;
            break;
        }
        linhaErro=i;

        // Controle de chaves do algoritmo **************************************
        if(strncmp(txtAlgoritmo[i],"}",1)==0){
            numChavesFechadas++;
            linhaErro=255;
        }

        //Comado ANDA(ESQUERDA ou DIREITA) **************************************
        else if(strncmp(txtAlgoritmo[i],"ANDAR(",6)==0){
            vetorInstrucao[nInstrucao].comando=RUN;
            vetorInstrucao[nInstrucao].iteracao=1;
            vetorInstrucao[nInstrucao].P=&cat;
            if(strncmp(&txtAlgoritmo[i][6],"DIREITA);",9)==0){
                vetorInstrucao[nInstrucao].direcao=RIGHT;
                nInstrucao++;
                linhaErro=255;
            }else if(strncmp(&txtAlgoritmo[i][6],"ESQUERDA);",10)==0){
                vetorInstrucao[nInstrucao].direcao=LEFT;
                nInstrucao++;
                linhaErro=255;
            }
            else if(strncmp(&txtAlgoritmo[i][6],"CIMA);",6)==0){
                vetorInstrucao[nInstrucao].direcao=UP;
                nInstrucao++;
                linhaErro=255;
            }
            else if(strncmp(&txtAlgoritmo[i][6],"BAIXO);",7)==0){
                vetorInstrucao[nInstrucao].direcao=DOWN;
                nInstrucao++;
                linhaErro=255;
            }
        }// *************************************************************************

        //Comado PULA(ESQUERDA ou DIREITA) **************************************
        else if(strncmp(txtAlgoritmo[i],"PULAR(",6)==0){
            vetorInstrucao[nInstrucao].comando=JUMP;
            vetorInstrucao[nInstrucao].iteracao=1;
            vetorInstrucao[nInstrucao].P=&cat;
            if(strncmp(&txtAlgoritmo[i][6],"DIREITA);",9)==0){
                vetorInstrucao[nInstrucao].direcao=RIGHT;
                nInstrucao++;
                linhaErro=255;
            }else if(strncmp(&txtAlgoritmo[i][6],"ESQUERDA);",10)==0){
                vetorInstrucao[nInstrucao].direcao=LEFT;
                nInstrucao++;
                linhaErro=255;
            }
            else if(strncmp(&txtAlgoritmo[i][6],"CIMA);",6)==0){
                vetorInstrucao[nInstrucao].direcao=UP;
                nInstrucao++;
                linhaErro=255;
            }
            else if(strncmp(&txtAlgoritmo[i][6],"BAIXO);",7)==0){
                vetorInstrucao[nInstrucao].direcao=DOWN;
                nInstrucao++;
                linhaErro=255;
            }
        }// *************************************************************************

        //Comado PEGAR **************************************
        else if(strncmp(txtAlgoritmo[i],"PEGAR();",8)==0){
            vetorInstrucao[nInstrucao].comando=PEGA;
            vetorInstrucao[nInstrucao].iteracao=1;
            vetorInstrucao[nInstrucao].P=&cat;
            vetorInstrucao[nInstrucao].acao=IDLE;
            vetorInstrucao[nInstrucao].direcao=RIGHT;
            vetorInstrucao[nInstrucao].objeto = &chave;

            nInstrucao++;
            linhaErro=255;
        }

        //Comado ABRIR **************************************
        else if(strncmp(txtAlgoritmo[i],"ABRIR();",8)==0){
            vetorInstrucao[nInstrucao].comando=ABRE;
            vetorInstrucao[nInstrucao].iteracao=1;
            vetorInstrucao[nInstrucao].P=&cat;
            vetorInstrucao[nInstrucao].acao=IDLE;
            vetorInstrucao[nInstrucao].direcao=RIGHT;
            vetorInstrucao[nInstrucao].objeto = &casa2;

            nInstrucao++;
            linhaErro=255;
        }//**************************************************

        //Comado REPETE **************************************
        else if(strncmp(txtAlgoritmo[i],"REPETIR(",8)==0){
            vetorInstrucao[nInstrucao].comando=REPETICAO;
            vetorInstrucao[nInstrucao].contIteracao=0;
            //Leitura do valor correpondente a iteracao e Busca por "){"
            ok=0;
            strcpy(aux,"a");
            for(ii=8;ii<TAM_LIN_ALGO;ii++){
                if(strncmp(&txtAlgoritmo[i][ii],"){",2)==0){
                    numChavesAbertas++;//Controle de chaves no algoritmo
                    ok=1; //Encontrou "){"  e sai do laco for
                    break;
                }else{
                    aux[ii-8]=txtAlgoritmo[i][ii];
                    aux[ii-8+1]='\0';
                }
            }
            //Conversao num. de iteracoes (string para int) e validacao numerica
            vetorInstrucao[nInstrucao].qtdeIteracao = atoi(aux);
            if(vetorInstrucao[nInstrucao].qtdeIteracao==0)ok=0;

            //Computacao do numero de comandos dentro da repeticao
            possui_SE=0;
            vetorInstrucao[nInstrucao].qtdeComandos=0;
            for(ii=i+1;ii<=QTDE_LIN_ALGO;ii++){
                if(strncmp(txtAlgoritmo[ii],"SE",2)==0){
                        possui_SE++;
                        vetorInstrucao[nInstrucao].qtdeComandos++;
                }
                else if((strncmp(txtAlgoritmo[ii],"}",1)==0)&&(possui_SE==0)){
                        break;
                }else if(strncmp(txtAlgoritmo[ii],"}",1)==0){
                    possui_SE--;
                }else vetorInstrucao[nInstrucao].qtdeComandos++;
            }

        //Se Instrucao foi compilada sem erros
        if(ok){
            nInstrucao++;
            linhaErro=255;
            }
        }// *************************************************************************

        //Comando CONDICIONAL SE**************************************
        else if(strncmp(txtAlgoritmo[i],"SE(",3)==0){
            vetorInstrucao[nInstrucao].comando=CONDICIONAL_SE;
            vetorInstrucao[nInstrucao].P=&cat;
            //Leitura do valor correpondente a iteracao e Busca por "){"
            ok=0;
            strcpy(aux,"a");
            for(ii=3;ii<TAM_LIN_ALGO;ii++){
                if(strncmp(&txtAlgoritmo[i][ii],"){",2)==0){
                    numChavesAbertas++;//Controle de chaves no algoritmo
                    ok=1; //Encontrou "){"  e sai do laco for
                    break;
                }else{ //Enquanto nao encontra "){"  ,   copia valores de dentro dos parenteses para variavel aux
                    aux[ii-3]=txtAlgoritmo[i][ii];
                    aux[ii-3+1]='\0';
                }
            }
            //IDENTIFICA OBJETO (PARAMETRO DO IF)
            if(strcmp(aux,"CHAVE")==0)vetorInstrucao[nInstrucao].objeto=&chave;
            else if(strcmp(aux,"BANANA")==0)vetorInstrucao[nInstrucao].objeto=&banana;
            else ok=0;

            //Computacao do numero de comandos dentro da Condicional
            possui_SE=0;
            vetorInstrucao[nInstrucao].qtdeComandos=0;
            for(ii=i+1;ii<=QTDE_LIN_ALGO;ii++){
                if(strncmp(txtAlgoritmo[ii],"REPETIR(",8)==0){
                        possui_SE++;
                        vetorInstrucao[nInstrucao].qtdeComandos++;
                }
                else if((strncmp(txtAlgoritmo[ii],"}",1)==0)&&(possui_SE==0)){
                        break;
                }else if(strncmp(txtAlgoritmo[ii],"}",1)==0){
                    possui_SE--;
                }else vetorInstrucao[nInstrucao].qtdeComandos++;
            }


        if(ok){
            nInstrucao++;
            linhaErro=255;
            }
        }// *************************************************************************


        //Em caso de erro, a linhaErro tera valor diferente de 255
        if(linhaErro!=255){
            return linhaErro;
        }
    }
    if(numChavesAbertas != numChavesFechadas) return 256;

    //Tudo ok, retorna linhaErro = 255
    return linhaErro;
}

void processadorInstrucao(InstrucaoPadrao *instrucao){
    int aux = Linha;

    switch(instrucao->comando){
    case RUN:
        if(acao(instrucao->P, RUN, instrucao->iteracao, instrucao->direcao))Linha++;
        //Se for o ultimo comando dentro de uma repeticao, volta a linha da repeticao
        //Se nao for o ultimo ou nao estiver dentro de uma repeticao, vai para proxima Linha
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Se chegou ao fim da condicional, pula a chave da condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case JUMP:
        if(acao(instrucao->P, JUMP, instrucao->iteracao, instrucao->direcao))Linha++;

        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case IDLE:
        if(acao(instrucao->P, IDLE, instrucao->iteracao, instrucao->direcao))Linha++;
        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case DEAD:
        if(acao(instrucao->P, DEAD, instrucao->iteracao, instrucao->direcao))Linha++;
        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case SLIDE:
        if(acao(instrucao->P, SLIDE, instrucao->iteracao, instrucao->direcao))Linha++;
        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case PEGA:
        if(acao(instrucao->P, IDLE, instrucao->iteracao, instrucao->direcao))Linha++;
        if((instrucao->P->posicaoX==instrucao->objeto->posicaoX)&&
           (instrucao->P->posicaoY==instrucao->objeto->posicaoY)&&
           (instrucao->P->contBlocos>=instrucao->iteracao)){
            instrucao->objeto->posicaoX=5*DESLOCAMENTO;
            instrucao->objeto->posicaoY=0;
        }
        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case ABRE:
        if(acao(instrucao->P, IDLE, instrucao->iteracao, instrucao->direcao))Linha++;
        if((instrucao->P->posicaoX==instrucao->objeto->posicaoX)&&
           (instrucao->P->posicaoY==instrucao->objeto->posicaoY)&&
           (instrucao->P->contBlocos>=instrucao->iteracao)){
            strcpy(casa2.texto,"ABERTA");
        }

        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
                marcador.contador++;//Alinha contador ao codigo
        }
        break;
    case CONDICIONAL_SE:
        //Atribui localizacao da condicional
        LinhaCondicional=Linha;
        LinhaFimCondicional=Linha+instrucao->qtdeComandos+1;

        if((instrucao->P->posicaoX==instrucao->objeto->posicaoX)&&
           (instrucao->P->posicaoY==instrucao->objeto->posicaoY)){
            condicionalAtiva=1;
            Linha++;
        }else{
            Linha = LinhaFimCondicional;
            condicionalAtiva=0;
            //Alinha marcador
            marcador.contador++;
        }

        //Controle de repeticao
        if((repeticaoAtiva)&&(Linha==LinhaFimRepeticao)){
                Linha=LinhaRepeticao;
        }
        break;
    case REPETICAO:
        //Atribui localizacao da repeticao (Na primeira iteracao)
        if(instrucao->contIteracao == 0){
                LinhaRepeticao=Linha;
                LinhaFimRepeticao=Linha+instrucao->qtdeComandos+1;
                repeticaoAtiva=1;
        }
        //Se repeticao ainda nao foi satizfeita...
        if(instrucao->contIteracao < instrucao->qtdeIteracao){
            instrucao->contIteracao++;
            Linha++;
        //Se repeticao foi satizfeita...
        }else{
            Linha=LinhaFimRepeticao;
            repeticaoAtiva=0;
            //Alinha contador ao codigo
            marcador.contador++;
        }
        //Controle de condicional
        if((condicionalAtiva)&&(Linha==LinhaFimCondicional)){
                condicionalAtiva=0;
        }
        break;
    }//FIM DO SWITCH-CASE


    if(Linha!=aux){
        printf("\n-----\nLinha: %i",Linha);
        printf("  -  Comando: %i",instrucao->comando);
        printf("  -  Qtde-Comando: %i",instrucao->qtdeComandos);
    }
}

void telaJogo(){
    if(Linha<nInstrucao){
        processadorInstrucao(&vetorInstrucao[Linha]);
    }
}

void telaQuadro(){
    int i,j,linhaErro=0;
    char msg[20]={"ERRO NA LINHA: "},erro[5];

    //Movimentos do Quadro
    marcador.posicaoY= (backgroundQuadro.posicaoFontY)+(backgroundQuadro.tamanhoFont*(Linha+marcador.contador+1))+(ESPACAMENTO_LIN*(Linha+marcador.contador+1));

    //Varredura teclado
    tratamentoTeclado();

    if(mouseClick(btnCompilar)){
        linhaErro = compilador();
        if(linhaErro < 255){
            nInstrucao=0;
            itoa(linhaErro,erro,10);
            strcat(msg,erro);
            al_show_native_message_box(quadro,"ERRO DE COMPILACAO",msg,"",NULL,ALLEGRO_MESSAGEBOX_OK_CANCEL);
        }else if(linhaErro == 256){
            nInstrucao=0;
            al_show_native_message_box(quadro,"ERRO DE COMPILACAO","VERIFICAR CHAVES {}","",NULL,ALLEGRO_MESSAGEBOX_OK_CANCEL);
        }else{
            marcador.ativo=1;
            marcador.contador=0;
        }
    }

    if(mouseClick(btnLimpar)){
        //reset gato
        cat.posicaoX=180;
        cat.posicaoY=180;
        cat.contSprite=0;
        cat.spriteAtual=0;
        cat.velocidadeX=2;
        cat.velocidadeY=2;
        cat.velocidadeSprite=59;
        cat.ativo=1;
        cat.contDesloc=0;
        cat.contBlocos=0;

        marcador.ativo=0;

        if(al_show_native_message_box(quadro,"ATENCAO!!","DESEJA APAGAR O CODIGO?","",NULL,ALLEGRO_MESSAGEBOX_YES_NO)){
            for(i=0;i<QTDE_LIN_ALGO;i++){
                for(j=0;j<TAM_LIN_ALGO;j++){
                    txtAlgoritmo[i][j]='\0';
                }
            }
            linhaAlgoritmo=0;
            colunaAlgoritmo=0;
        }
    }

    if(mousePosicao(btnCompilar)){
        btnCompilar.inverso=1;
    }else btnCompilar.inverso=0;

    if(mousePosicao(btnLimpar)){
        btnLimpar.inverso=1;
    }else btnLimpar.inverso=0;
}

int main(void){
    int desenha = 1,i,j;
    int sair = 0;

    if (!inicializar()){
        finalizar();
        return -1;
    }

    if(mouseClick(btnLimpar)){
        for(i=0;i<QTDE_LIN_ALGO;i++){
            for(j=0;j<TAM_LIN_ALGO;j++){
                txtAlgoritmo[i][j]='\0';
            }
        }
    }
    linhaAlgoritmo=0;
    colunaAlgoritmo=0;

    while(!sair){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        /* -- EVENTOS -- */
        //TIMER...
        if(evento.type == ALLEGRO_EVENT_TIMER){
            telaJogo();
            telaQuadro();
            mouse.click=0;
        }
        //MOUSE CLICK...
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            mouse.click=1;
            ajustaPosMouse(evento.mouse.x, evento.mouse.y);
        }
        //MOUSE AXES...
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES){
            ajustaPosMouse(evento.mouse.x, evento.mouse.y);
        }
        //TECLADO...
        else if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
            teclado.codTecla = evento.keyboard.keycode;  //Codigo da tecla
            teclado.codASCII = (char)evento.keyboard.unichar; // Codigo ASCII
            if((teclado.codASCII >= 97)&&(teclado.codASCII <= 122))teclado.codASCII-=32;
            if((teclado.codASCII >= 32)&&(teclado.codASCII <= 126)){
                teclado.caracterPendente = 1;
                teclado.teclaPress = 1;
            }else if((teclado.codASCII == 8)||(teclado.codASCII == 13)){
                teclado.teclaPress=1;
            }else if((teclado.codTecla==ALLEGRO_KEY_UP)||
                     (teclado.codTecla==ALLEGRO_KEY_DOWN)||
                     (teclado.codTecla==ALLEGRO_KEY_RIGHT)||
                     (teclado.codTecla==ALLEGRO_KEY_LEFT)){
                         teclado.teclaPress=1;
            }

        }
        //JANELAS...
        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
        }else if(evento.type == ALLEGRO_EVENT_DISPLAY_RESIZE){

            if(evento.display.source == quadro){
                mouse.ajusteX =  (float)(evento.display.width);
                mouse.ajusteY =  (float)(evento.display.height);
            }
        }
        /* -- ATUALIZA TELA -- */
        desenha=1;
        if(desenha && al_is_event_queue_empty(fila_eventos)) {
        desenha = drawTelaJogo(&cat);
        }
    }
    finalizar();
    return 0;
}
