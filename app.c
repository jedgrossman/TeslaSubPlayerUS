#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <subghz/subghz.h>

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str(canvas, 5, 20, "Tesla Launcher");
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str(canvas, 5, 40, "UP:   Play tesla1.sub");
    canvas_draw_str(canvas, 5, 55, "DOWN: Play tesla2.sub");
    canvas_draw_str(canvas, 5, 70, "BACK: Exit");
}

static void play_file(const char* path) {
    FURI_LOG_I("TESLAAPP", "Playing %s", path);

    SubGhz* sub = furi_record_open(RECORD_SUBGHZ);
    SubGhzTxRx* tx = subghz_txrx_alloc();

    if(subghz_txrx_load_from_file(tx, path)) {
        subghz_txrx_start(tx);
    } else {
        FURI_LOG_E("TESLAAPP", "Failed to load: %s", path);
    }

    subghz_txrx_stop(tx);
    subghz_txrx_free(tx);
    furi_record_close(RECORD_SUBGHZ);
}

static void input_callback(InputEvent* event, void* ctx) {
    FuriMessageQueue* queue = ctx;
    furi_message_queue_put(queue, event, 0);
}

int32_t tesla_app_start(void* p) {
    UNUSED(p);

    FuriMessageQueue* input_queue =
        furi_message_queue_alloc(8, sizeof(InputEvent));

    ViewPort* vp = view_port_alloc();
    view_port_draw_callback_set(vp, draw_callback, NULL);
    view_port_input_callback_set(vp, input_callback, input_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, vp, GuiLayerFullscreen);

    bool running = true;
    InputEvent event;

    while(running) {
        if(furi_message_queue_get(input_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                    case InputKeyUp:
                        play_file("/ext/subghz/tesla1.sub");
                        break;

                    case InputKeyDown:
                        play_file("/ext/subghz/tesla2.sub");
                        break;

                    case InputKeyBack:
                        running = false;
                        break;

                    default:
                        break;
                }
            }
        }
    }

    gui_remove_view_port(gui, vp);
    view_port_free(vp);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(input_queue);

    return 0;
}
