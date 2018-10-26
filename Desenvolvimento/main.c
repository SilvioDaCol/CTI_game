#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>


#define FPS             60.0
#define LARGURA_TELA    1000
#define ALTURA_TELA     1000
#define LARGURA_QUADRO  330
#define ALTURA_QUADRO   1000
#define RUN             0
#define JUMP            1
#define IDLE            2
#define DEAD            3
#define SLIDE           4
#define LEFT            0
#define RIGHT           1
#define UP              2
#define DOWN            3
#define DESLOCAMENTO    91 // = TELA(1000) / NUMERO DE QUADRADOS (11)
#define QTDE_LIN_ALGO   10
#define TAM_LIN_ALGO    16
#define ESPACAMENTO_LIN 15

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_DISPLAY *quadro =NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;

typedef struct{ //Objetos
    //Caraceristicas do Objeto
    ALLEGRO_BITMAP *img;
    int posicaoX;
    int posicaoY;
    int contador;
    int velocidade;
    int ativo;
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
}InputDevice;

/* PERSONAGENS E OBJETOS ------------------------------------*/
Objeto backgroundJogo;
Objeto backgroundQuadro;
Objeto marcador;
Objeto btnCompilar;
Objeto btnLimpar;
Personagem cat;
InputDevice teclado;

/* VARIAVEIS GLOBAIS ------------------------------------*/
int Linha = 0;
char txtAlgoritmo[QTDE_LIN_ALGO][TAM_LIN_ALGO+1];
int linhaAlgoritmo=0, colunaAlgoritmo=0;

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
    backgroundQuadro.tamanhoFont = 30;
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
    marcador.ativo = 1;
    marcador.posicaoX = 0;
    marcador.posicaoY = backgroundQuadro.posicaoFontY-5;
    marcador.velocidade = 30;

    //carrega o botao Compilar
    btnCompilar.img = al_load_bitmap("img/btnCompilar.png");
    if (!btnCompilar.img){
        return 0;
    }
    btnCompilar.ativo=1;
    btnCompilar.posicaoX= (LARGURA_QUADRO/3)-50;
    btnCompilar.posicaoY= ALTURA_QUADRO-al_get_bitmap_height(btnCompilar.img)-10;

    //carrega o botao Limpar
    btnLimpar.img = al_load_bitmap("img/btnLimpar.png");
    if (!btnLimpar.img){
        return 0;
    }
    btnLimpar.ativo=1;
    btnLimpar.posicaoX= (LARGURA_QUADRO/3)*2;
    btnLimpar.posicaoY= ALTURA_QUADRO-al_get_bitmap_height(btnLimpar.img)-10;

    //Inicializacao teclado
    teclado.backSpace=0;
    teclado.caracterPendente=0;
    teclado.codASCII=NULL;
    teclado.enter=0;
    teclado.espaco=0;
    teclado.tab=0;
    teclado.teclaPress=0;
    teclado.cont=0;
    teclado.cursor='_';

    return 1;
}

int initPersonagens(){
    int i;

        //posicao X Y da janela em que sera mostrado o sprite
    cat.acao=RUN;
    //COL1=60(X) COL2=390(X)  LIN1=35(Y) LIN2=(Y)
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

    //carrega a folha de sprites na variavel (IDLE - CAT)
    /*
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
    */
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
    /*
    //carrega a folha de sprites na variavel (JUMP - CAT)
    cat.frames_spriteJump = 10;
    cat.spriteImg[JUMP][0] = al_load_bitmap("img/cat/Jump (1).png");
    cat.spriteImg[JUMP][1] = al_load_bitmap("img/cat/Jump (2).png");
    cat.spriteImg[JUMP][2] = al_load_bitmap("img/cat/Jump (3).png");
    cat.spriteImg[JUMP][3] = al_load_bitmap("img/cat/Jump (4).png");
    cat.spriteImg[JUMP][4] = al_load_bitmap("img/cat/Jump (5).png");
    cat.spriteImg[JUMP][5] = al_load_bitmap("img/cat/Jump (6).png");
    cat.spriteImg[JUMP][6] = al_load_bitmap("img/cat/Jump (7).png");
    cat.spriteImg[JUMP][7] = al_load_bitmap("img/cat/Jump (8).png");
    cat.spriteImg[JUMP][8] = al_load_bitmap("img/cat/Jump (9).png");
    cat.spriteImg[JUMP][9] = al_load_bitmap("img/cat/Jump (10).png");
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
    */
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

    // Atribui o cursor padrão do sistema para ser usado
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
    //desenha sprite na posicao X Y da janela, a partir da regiao X Y da folha
    if(P->ativo)al_draw_bitmap(P->spriteImg[P->acao][P->spriteAtual], P->posicaoX+(DESLOCAMENTO/2)-(al_get_bitmap_width(P->spriteImg[P->acao][P->spriteAtual])/2), P->posicaoY-(al_get_bitmap_height(P->spriteImg[P->acao][P->spriteAtual])-DESLOCAMENTO), P->sentido);
    al_flip_display();


    //Coloca tela do compilador (quadro) como alvo **********************************************
    al_set_target_bitmap(al_get_backbuffer(quadro));
    //al_clear_to_color(al_map_rgb(20,100,20));
    if(backgroundQuadro.ativo)al_draw_scaled_bitmap(backgroundQuadro.img,0.0,0.0,al_get_bitmap_width(backgroundQuadro.img), al_get_bitmap_height(backgroundQuadro.img), 0.0,0.0, (float)al_get_display_width(quadro), (float)al_get_display_height(quadro),0);
    if(backgroundQuadro.fontAtivo){
        for(i=0;i<QTDE_LIN_ALGO;i++){
            al_draw_textf(backgroundQuadro.font,al_map_rgb(backgroundQuadro.textColour_R,backgroundQuadro.textColour_G,backgroundQuadro.textColour_B),backgroundQuadro.posicaoFontX,(backgroundQuadro.posicaoFontY)+(backgroundQuadro.tamanhoFont*i)+(ESPACAMENTO_LIN*i),0,&txtAlgoritmo[i][0]);
        }
    }
    if(btnCompilar.ativo)al_draw_bitmap(btnCompilar.img,btnCompilar.posicaoX,btnCompilar.posicaoY,0);
    if(btnLimpar.ativo)al_draw_bitmap(btnLimpar.img,btnLimpar.posicaoX,btnLimpar.posicaoY,0);
    //marcador
    if(marcador.ativo)al_draw_rectangle(marcador.posicaoX,marcador.posicaoY,marcador.posicaoX+al_get_display_width(quadro),marcador.posicaoY+backgroundQuadro.tamanhoFont+10,al_map_rgb(20, 255, 20),6);

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

void acao(Personagem *P, int movimento, int iteracao, int Direcao){
    if(P->contBlocos>=iteracao){
        Linha++;
        P->contBlocos=0;
        P->contDesloc=0;
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
    }
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
            txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo] = ' ';
            teclado.cursorAtivo=0;
        }else {
            txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]= teclado.cursor;
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
            //al_show_native_message_box(janela,"TECLADO!!","TECLADO","TECLADO",NULL,ALLEGRO_MESSAGEBOX_OK_CANCEL);
        }else{//---------------------------------Se foi tecla de comando

            if(teclado.codASCII==8){ //BAKSPACE
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo] = ' ';//apaga cursor
                colunaAlgoritmo--;
                if(colunaAlgoritmo<0){
                    linhaAlgoritmo--;
                    colunaAlgoritmo=TAM_LIN_ALGO-1;
                    if(linhaAlgoritmo<0){
                        linhaAlgoritmo=0;
                        colunaAlgoritmo=0;
                    }
                }
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo]= ' ';
            }else if(teclado.codASCII==13){//ENTER
                txtAlgoritmo[linhaAlgoritmo][colunaAlgoritmo] = ' ';//apaga cursor
                if(linhaAlgoritmo<QTDE_LIN_ALGO-1){
                    linhaAlgoritmo++;
                    colunaAlgoritmo=0;
                }
            }
        }

        teclado.teclaPress=0;
    }
}

void telaJogo(){
    // Movimentos da gato (Tela do jogo)
    if(Linha==0)acao(&cat, RUN, 6,RIGHT);
    else if(Linha==1)acao(&cat, SLIDE, 6, DOWN);
    else if(Linha==2)acao(&cat, RUN, 3, LEFT);
    else if(Linha==3)acao(&cat, SLIDE, 3, LEFT);
    else if(Linha==4)acao(&cat, RUN, 6, UP);
    else{
        Linha=0;
    }
}

void telaQuadro(){
    //Movimentos do Quadro
    if(marcador.contador>marcador.velocidade){
        marcador.contador = 0;
        marcador.posicaoY+=backgroundQuadro.tamanhoFont+ESPACAMENTO_LIN;
        if(marcador.posicaoY>ALTURA_TELA-100)marcador.posicaoY=backgroundQuadro.posicaoFontY-5;
    }else marcador.contador++;

    tratamentoTeclado();
}

int main(void){
    int desenha = 1,i;
    int sair = 0;
    char c[10];

    if (!inicializar()){
        finalizar();
        return -1;
    }

    for(i=0;i<QTDE_LIN_ALGO;i++){
        itoa(i, c, 10);
        strcpy(&txtAlgoritmo[i][0],"");
        //strcat(&txtAlgoritmo[i][0], c);
    }

    while(!sair){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        /* -- EVENTOS -- */
        //TIMER...
        if(evento.type == ALLEGRO_EVENT_TIMER){
            telaJogo();
            telaQuadro();
        }
        //MOUSE...
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
        }
        //TECLADO...
        if (evento.type == ALLEGRO_EVENT_KEY_CHAR){
            teclado.codASCII = (char)evento.keyboard.unichar;
            if((teclado.codASCII >= 97)&&(teclado.codASCII <= 122))teclado.codASCII-=32;
            if((teclado.codASCII >= 32)&&(teclado.codASCII <= 126)){
                teclado.caracterPendente = 1;
                teclado.teclaPress = 1;
            }else if((teclado.codASCII == 8)||(teclado.codASCII == 13)){
                teclado.teclaPress=1;
            }

        }
        //JANELAS...
        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
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
