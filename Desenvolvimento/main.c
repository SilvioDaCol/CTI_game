#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#define FPS             60.0
#define LARGURA_TELA    1000
#define ALTURA_TELA     1000
#define RUN             0
#define JUMP            1
#define IDLE            2
#define DEAD            3
#define SLIDE           4
#define LEFT            0
#define RIGHT           1
#define UP              2
#define DOWN            3
#define DESLOCAMENTO    91 // = TELA(1000) / NUMERO DE QUADRADOS (11) / 10(NUMERO DE PASSOS)


ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_DISPLAY *quadro =NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;

typedef struct{ //Objetos
    ALLEGRO_BITMAP *img;
    int posicaoX;
    int posicaoY;
    int ativo;
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

Objeto fundo;
Personagem cat;
int Linha = 0;
int rectX=0;
int rectY=0;

void error_msg(char *text){
	al_show_native_message_box(janela,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int initObjetos(){
    //carrega o fundo
    fundo.img = al_load_bitmap("img/background.jpg");
    if (!fundo.img){
        return 0;
    }
    fundo.ativo=1;
    fundo.posicaoX=0;
    fundo.posicaoY=0;


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

    quadro = al_create_display(monitor.x2-LARGURA_TELA,ALTURA_TELA);
    if(!quadro) {
        error_msg("Falha ao criar quadro");
        return 0;
    }
    al_set_window_title(quadro, "Compilador");
    al_set_window_position(quadro,LARGURA_TELA,monitor.y1);
    al_clear_to_color(al_map_rgb(20,100,20));

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
    //Coloca tela do jogo (janela) como alvo
    al_set_target_bitmap(al_get_backbuffer(janela));
    //desenha o fundo na tela
    if(fundo.ativo)al_draw_scaled_bitmap(fundo.img,0.0,0.0, al_get_bitmap_height(fundo.img), al_get_bitmap_width(fundo.img), 0.0,0.0, (float)al_get_display_height(janela), (float)al_get_display_width(janela),0);
    //desenha sprite na posicao X Y da janela, a partir da regiao X Y da folha
    if(P->ativo)al_draw_bitmap(P->spriteImg[P->acao][P->spriteAtual], P->posicaoX+(DESLOCAMENTO/2)-(al_get_bitmap_width(P->spriteImg[P->acao][P->spriteAtual])/2), P->posicaoY-(al_get_bitmap_height(P->spriteImg[P->acao][P->spriteAtual])-DESLOCAMENTO), P->sentido);
    al_flip_display();
    //Coloca tela do compilador (quadro) como alvo
    al_set_target_bitmap(al_get_backbuffer(quadro));
    al_clear_to_color(al_map_rgb(20,100,20));
    al_draw_rectangle(rectX,rectY,rectX+al_get_display_width(quadro),rectY+90,al_map_rgb(255, 0, 255),6);

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
    al_destroy_bitmap(fundo.img);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_display(quadro);
    al_destroy_event_queue(fila_eventos);
}

int main(void){
    int desenha = 1,i=0;
    int sair = 0;

    if (!inicializar()){
        finalizar();
        return -1;
    }

    while(!sair){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        /* -- EVENTOS -- */
        //TIMER...
        if(evento.type == ALLEGRO_EVENT_TIMER){

            // ovimentos da gato (Tela do jogo)
            if(Linha==0)acao(&cat, RUN, 6,RIGHT);
            else if(Linha==1)acao(&cat, SLIDE, 6, DOWN);
            else if(Linha==2)acao(&cat, RUN, 3, LEFT);
            else if(Linha==3)acao(&cat, SLIDE, 3, LEFT);
            else if(Linha==4)acao(&cat, RUN, 6, UP);
            else{
                Linha=0;
            }

            //Movimentos do Quadro
            if(i>30){
                i = 0;
                rectY+=DESLOCAMENTO;
                if(rectY>ALTURA_TELA)rectY=0;
            }else i++;


        //MOUSE...
        }else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

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
