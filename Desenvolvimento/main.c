#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

#define FPS             60.0
#define LARGURA_TELA    1024
#define ALTURA_TELA     383
#define RUN             0
#define JUMP            1
#define IDLE            2
#define DEAD            3

ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_TIMER *timer = NULL;

typedef struct{
    ALLEGRO_BITMAP *img;
    int posicaoX;
    int posicaoY;
    int ativo;
}Objeto;

typedef struct{
    ALLEGRO_BITMAP *spriteImg[4][10];
    int acao;
    int spriteAtual;
    int contSprite;
    float velocidadeSprite;
    int frames_spriteRun;
    int frames_spriteJump;
    int frames_spriteDead;
    int frames_spriteIdle;
    int posicaoX;
    int posicaoY;
    int velocidadeX;
    int velocidadeY;
    int ativo;
}Personagem;

Objeto fundo;
Personagem girl;
int contEventos=0;


void error_msg(char *text){
	al_show_native_message_box(janela,"ERRO",
		"Ocorreu o seguinte erro e o programa sera finalizado:",
		text,NULL,ALLEGRO_MESSAGEBOX_ERROR);
}

int initObjetos(){
    //carrega o fundo
    fundo.img = al_load_bitmap("img/background.png");
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
    //carrega a folha de sprites na variavel (IDLE - GIRL)
    girl.frames_spriteIdle = 10;
    girl.spriteImg[IDLE][0] = al_load_bitmap("img/girl/Idle (1).png");
    girl.spriteImg[IDLE][1] = al_load_bitmap("img/girl/Idle (2).png");
    girl.spriteImg[IDLE][2] = al_load_bitmap("img/girl/Idle (3).png");
    girl.spriteImg[IDLE][3] = al_load_bitmap("img/girl/Idle (4).png");
    girl.spriteImg[IDLE][4] = al_load_bitmap("img/girl/Idle (5).png");
    girl.spriteImg[IDLE][5] = al_load_bitmap("img/girl/Idle (6).png");
    girl.spriteImg[IDLE][6] = al_load_bitmap("img/girl/Idle (7).png");
    girl.spriteImg[IDLE][7] = al_load_bitmap("img/girl/Idle (8).png");
    girl.spriteImg[IDLE][8] = al_load_bitmap("img/girl/Idle (9).png");
    girl.spriteImg[IDLE][9] = al_load_bitmap("img/girl/Idle (10).png");
    for(i=0;i<girl.frames_spriteIdle;i++)
    {
        if (!girl.spriteImg[IDLE][i]){
            return 0;
        }
    }
    //carrega a folha de sprites na variavel (RUN - GIRL)
    girl.frames_spriteRun = 8;
    girl.spriteImg[RUN][0] = al_load_bitmap("img/girl/Run (1).png");
    girl.spriteImg[RUN][1] = al_load_bitmap("img/girl/Run (2).png");
    girl.spriteImg[RUN][2] = al_load_bitmap("img/girl/Run (3).png");
    girl.spriteImg[RUN][3] = al_load_bitmap("img/girl/Run (4).png");
    girl.spriteImg[RUN][4] = al_load_bitmap("img/girl/Run (5).png");
    girl.spriteImg[RUN][5] = al_load_bitmap("img/girl/Run (6).png");
    girl.spriteImg[RUN][6] = al_load_bitmap("img/girl/Run (7).png");
    girl.spriteImg[RUN][7] = al_load_bitmap("img/girl/Run (8).png");
    girl.spriteImg[RUN][8] = al_load_bitmap("img/girl/Run (8).png");
    girl.spriteImg[RUN][9] = al_load_bitmap("img/girl/Run (8).png");
    for(i=0;i<girl.frames_spriteRun;i++)
    {
        if (!girl.spriteImg[RUN][i]){
            return 0;
        }
    }
    //carrega a folha de sprites na variavel (JUMP - GIRL)
    girl.frames_spriteJump = 10;
    girl.spriteImg[JUMP][0] = al_load_bitmap("img/girl/Jump (1).png");
    girl.spriteImg[JUMP][1] = al_load_bitmap("img/girl/Jump (2).png");
    girl.spriteImg[JUMP][2] = al_load_bitmap("img/girl/Jump (3).png");
    girl.spriteImg[JUMP][3] = al_load_bitmap("img/girl/Jump (4).png");
    girl.spriteImg[JUMP][4] = al_load_bitmap("img/girl/Jump (5).png");
    girl.spriteImg[JUMP][5] = al_load_bitmap("img/girl/Jump (6).png");
    girl.spriteImg[JUMP][6] = al_load_bitmap("img/girl/Jump (7).png");
    girl.spriteImg[JUMP][7] = al_load_bitmap("img/girl/Jump (8).png");
    girl.spriteImg[JUMP][8] = al_load_bitmap("img/girl/Jump (9).png");
    girl.spriteImg[JUMP][9] = al_load_bitmap("img/girl/Jump (10).png");
    for(i=0;i<girl.frames_spriteJump;i++)
    {
        if (!girl.spriteImg[JUMP][i]){
            return 0;
        }
    }
    //carrega a folha de sprites na variavel (DEAD - GIRL)
    girl.frames_spriteDead = 10;
    girl.spriteImg[DEAD][0] = al_load_bitmap("img/girl/Dead (1).png");
    girl.spriteImg[DEAD][1] = al_load_bitmap("img/girl/Dead (2).png");
    girl.spriteImg[DEAD][2] = al_load_bitmap("img/girl/Dead (3).png");
    girl.spriteImg[DEAD][3] = al_load_bitmap("img/girl/Dead (4).png");
    girl.spriteImg[DEAD][4] = al_load_bitmap("img/girl/Dead (5).png");
    girl.spriteImg[DEAD][5] = al_load_bitmap("img/girl/Dead (6).png");
    girl.spriteImg[DEAD][6] = al_load_bitmap("img/girl/Dead (7).png");
    girl.spriteImg[DEAD][7] = al_load_bitmap("img/girl/Dead (8).png");
    girl.spriteImg[DEAD][8] = al_load_bitmap("img/girl/Dead (9).png");
    girl.spriteImg[DEAD][9] = al_load_bitmap("img/girl/Dead (10).png");
    for(i=0;i<girl.frames_spriteDead;i++)
    {
        if (!girl.spriteImg[DEAD][i]){
            return 0;
        }
    }
    //posicao X Y da janela em que sera mostrado o sprite
    girl.acao=IDLE;
    girl.posicaoX=0;
    girl.posicaoY=150;
    girl.contSprite=0;
    girl.spriteAtual=0;
    girl.velocidadeX=4;
    girl.velocidadeSprite=56;
    girl.velocidadeY=0;
    girl.ativo=1;

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

    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        error_msg("Falha ao criar temporizador");
        return 0;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if(!janela) {
        error_msg("Falha ao criar janela");
        al_destroy_timer(timer);
        return 0;
    }
    al_set_window_title(janela, "Projeto Wash - CTI Renato Archer");


    fila_eventos = al_create_event_queue();
    if(!fila_eventos) {
        error_msg("Falha ao criar fila de eventos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        return 0;
    }

    if(!initPersonagens()){
        error_msg("Falha ao carregar Personagens");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return 0;
    }


    /*
    //usa a cor rosa como transparencia
    al_convert_mask_to_alpha(folha_sprite,al_map_rgb(255,0,255));
    */

    if(!initObjetos()){
        error_msg("Falha ao carregar Objetos");
        al_destroy_timer(timer);
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        //al_destroy_bitmap(folha_sprite);
        return 0;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_start_timer(timer);

    return 1;
}

int drawTelaJogo(Personagem *P){
    //desenha o fundo na tela
    if(fundo.ativo)al_draw_bitmap(fundo.img,fundo.posicaoX,fundo.posicaoY, 0);
    //desenha sprite na posicao X Y da janela, a partir da regiao X Y da folha
    if(P->ativo)al_draw_bitmap(P->spriteImg[P->acao][P->spriteAtual], P->posicaoX, P->posicaoY, 0);

    al_flip_display();

    return 0;
}

void runPersonagem(Personagem *P){
    contEventos++;
    //Deslocamento do Fundo
    if(fundo.posicaoX <= -LARGURA_TELA )fundo.posicaoX=0;
    else fundo.posicaoX -= P->velocidadeX;
    //Movimentação Personagem (sprites)
    P->acao = RUN;
    if(P->spriteAtual>=P->frames_spriteRun) P->spriteAtual=0;
    else{
        if(P->contSprite<P->velocidadeSprite ){
            P->spriteAtual++;
            P->contSprite=FPS;
        }else P->contSprite--;
    }
}

void idlePersonagem(Personagem *P){
    //Movimentação Personagem (sprites)
    P->acao = IDLE;
    if(P->spriteAtual>=P->frames_spriteIdle-1) P->spriteAtual=0;
    else{
        if(P->contSprite<P->velocidadeSprite ){
            P->spriteAtual++;
            P->contSprite=FPS;
        }else P->contSprite--;
    }
}

void jumpPersonagem(Personagem *P){
    contEventos++;
    //Deslocamento do Fundo
    if(fundo.posicaoX <= -LARGURA_TELA )fundo.posicaoX=0;
    else fundo.posicaoX -= P->velocidadeX;
    //Movimentação Personagem (sprites)
    P->acao = JUMP;
    if(P->spriteAtual>=P->frames_spriteJump-1) P->spriteAtual=0;
    else{
        if(P->contSprite<P->velocidadeSprite ){
            P->spriteAtual++;
            P->contSprite=FPS;
        }else P->contSprite--;
    }
}

void deadPersonagem(Personagem *P){
    contEventos++;
    //Deslocamento do Fundo
    if(fundo.posicaoX <= -LARGURA_TELA )fundo.posicaoX=0;
    else fundo.posicaoX -= P->velocidadeX;
    //Movimentação Personagem (sprites)
    P->acao = DEAD;
    if(P->spriteAtual>=P->frames_spriteDead-1) P->spriteAtual=0;
    else{
        if(P->contSprite<P->velocidadeSprite ){
            P->spriteAtual++;
            P->contSprite=FPS;
        }else P->contSprite--;
    }
}

int main(void){
    int desenha = 1;
    int sair = 0;

    if (!inicializar()){
        return -1;
    }

    while(!sair){
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        /* -- EVENTOS -- */
        if(evento.type == ALLEGRO_EVENT_TIMER){

            //Correr por 8 frames:
            //if(contEventos<girl.frames_spriteRun) runPersonagem(&girl);

            //Correr ininterrupto
            //runPersonagem(&girl);

            //Respirar ininterrupto
            idlePersonagem(&girl);

            //Pular Ininterrupto
            //jumpPersonagem(&girl);

            //Cair Ininterrupto
            //deadPersonagem(&girl);

            desenha=1;
        }
        else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
        }

        /* -- ATUALIZA TELA -- */
        if(desenha && al_is_event_queue_empty(fila_eventos)) {
            desenha = drawTelaJogo(&girl);
        }
    }

    //al_destroy_bitmap(folha_sprite);
    al_destroy_bitmap(fundo.img);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}
