#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>

const int LARGURA_TELA = 640;
const int ALTURA_TELA = 480;

ALLEGRO_BITMAP *imagem = NULL;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos;
ALLEGRO_THREAD *thread = NULL;

bool inicializar();
bool carregar_arquivos();
void finalizar();
void *minha_thread(ALLEGRO_THREAD *thr, void *dados);
bool sair = false;

int main(void)
{
    if (!inicializar())
    {
        return -1;
    }

    if (!carregar_arquivos())
    {
        return -1;
    }

    // Cria a thread e a dispara
    thread = al_create_thread(minha_thread, NULL);
    al_start_thread(thread);

    al_draw_bitmap(imagem, 0, 0, 0 );
    al_flip_display();

    while (!sair)
    {
        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                sair = true;
            }
        }
    }

    finalizar();
    return 0;
}

bool inicializar()
{
    if (!al_init())
    {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return false;
    }

    if (!al_init_image_addon())
    {
        fprintf(stderr, "Falha ao inicializar add-on para imagens.\n");
        return false;
    }

    janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!janela)
    {
        return false;
    }
    al_set_window_title(janela, "Testando Threads");

    fila_eventos = al_create_event_queue();
    if (!fila_eventos)
    {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        al_destroy_display(janela);
        return false;
    }

    al_register_event_source(fila_eventos, al_get_display_event_source(janela));

    return true;
}

bool carregar_arquivos()
{
    imagem = al_load_bitmap("imagem.jpg");
    if (!imagem)
    {
        fprintf(stderr, "Falha ao carregar imagem.\n");
        al_destroy_display(janela);
        al_destroy_event_queue(fila_eventos);
        return false;
    }

    return true;
}

void finalizar()
{
    al_destroy_thread(thread);
    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(imagem);
    al_destroy_display(janela);
}

void *minha_thread(ALLEGRO_THREAD *thr, void *dados )
{
    while(!sair)
    {
        al_set_window_title(janela, "Thread disparada");
        al_rest(0.25);

        al_set_window_title(janela, "Thread disparada.");
        al_rest(0.25);

        al_set_window_title(janela, "Thread disparada. .");
        al_rest(0.25);

        al_set_window_title(janela, "Thread disparada. . .");
        al_rest(0.25);
    }

    return NULL;
}
