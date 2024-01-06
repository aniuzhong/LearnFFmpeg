#include <libavutil/avstring.h>
#include <libavutil/time.h>
#include <libavformat/avformat.h>

#include <SDL.h>
#include <SDL_thread.h>

const char program_name[] = "ffplay";

/* polls for possible required screen refresh at least this often, should be less than 1/fps */
#define REFRESH_RATE 0.01;

/* options specified by the user */
static const char *input_filename;

#define FF_QUIT_EVENT (SDL_USEREVENT + 2)

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_RendererInfo renderer_info = {0};
static SDL_AudioDeviceID audio_dev;

static void do_exit()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_Quit();
    av_log(NULL, AV_LOG_INFO, "%s", "");
    exit(0); 
}

/* display the current picture, if any */
static void video_display()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

static void refresh_loop_wait_event(SDL_Event *event)
{
    double remaining_time = 0.0;
    SDL_PumpEvents();
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
        SDL_PumpEvents();
        video_display();
    }
}

/* handle an event sent by the GUI*/
static void event_loop()
{
    SDL_Event event;

    for (;;) {
        refresh_loop_wait_event(&event);
        switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_q) {
                do_exit();
                break;
            }
            switch (event.key.keysym.sym) {
            case SDLK_f:
            case SDLK_p:
            case SDLK_SPACE:
            case SDLK_m:
            case SDLK_KP_MULTIPLY: 
            case SDLK_0:
            case SDLK_KP_DIVIDE:
            case SDLK_9:
            case SDLK_s:
            case SDLK_a:
            case SDLK_v:
            case SDLK_c:
            case SDLK_t:
            case SDLK_w:
            case SDLK_PAGEUP:
            case SDLK_PAGEDOWN:
            case SDLK_LEFT:
            case SDLK_RIGHT:
            case SDLK_UP:
            case SDLK_DOWN:
                printf("SDL_KEYDOWN\n");
            default:
                break;
            }
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_WINDOWEVENT:
            break;
        case SDL_QUIT:
        case FF_QUIT_EVENT:
            do_exit();
            break;
        default:
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int flags;

    // av_log_set_flags(AV_LOG_SKIP_REPEATED);

    // if (!input_filename) {
    //     av_log(NULL, AV_LOG_FATAL, "An input file must be specofied\n");
    //     av_log(NULL, AV_LOG_FATAL,
    //            "Use -h to get full help of, even better, run 'man %s'\n", program_name);
    //     exit(1);
    // }

    flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;

    if (SDL_Init(flags) != 0) {
        av_log(NULL, AV_LOG_FATAL, "Could not initialize SDL - %s\n", SDL_GetError());
        av_log(NULL, AV_LOG_FATAL, "(Did you set the DISPLAY variable?)\n");
        exit(1); 
    }

    SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
    SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

    window = SDL_CreateWindow(program_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (window) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            av_log(NULL, AV_LOG_WARNING, "Failed to initialize a hardware accelerated renderer: %s\n", SDL_GetError());
            renderer = SDL_CreateRenderer(window, -1, 0);
        } 
        if (renderer) {
            if (!SDL_GetRendererInfo(renderer, &renderer_info))
                av_log(NULL, AV_LOG_INFO, "Initialized %s renderer.\n", renderer_info.name);
        }
    }
    if (!window || !renderer || !renderer_info.num_texture_formats) {
        av_log(NULL, AV_LOG_FATAL, "Failed to create window or renderer: %s", SDL_GetError());
        do_exit();
    }

    event_loop();

    /* never return */

    return 0;
}