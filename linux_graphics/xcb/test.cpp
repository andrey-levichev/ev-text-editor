#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_gcontext_t gctx;
    xcb_generic_event_t *event;
    uint32_t mask;
    uint32_t values[2];
    bool done = false;
    xcb_rectangle_t rect = { 20, 20, 60, 60 };

    /* open connection with the server */
    connection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(connection)) 
    {
        printf("Cannot open display\n");
        exit(1);
    }
    
    /* get the first screen */
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    /* create black graphics context */
    gctx = xcb_generate_id(connection);
    window = screen->root;
    mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = screen->black_pixel;
    values[1] = 0;
    xcb_create_gc(connection, gctx, window, mask, values);

    /* create window */
    window = xcb_generate_id(connection);
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;
    xcb_create_window(connection, screen->root_depth, window, screen->root,
        10, 10, 100, 100, 1,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
        mask, values);

    /* map (show) the window */
    xcb_map_window(connection, window);
    xcb_flush(connection);

    /* event loop */
    while (!done && (event = xcb_wait_for_event(connection))) 
    {
        switch (event->response_type & ~0x80) 
        {
        case XCB_EXPOSE:    /* draw or redraw the window */
            xcb_poly_fill_rectangle(connection, window, gctx,  1, &rect);
            xcb_flush(connection);
            break;
        case XCB_KEY_PRESS:  /* exit on key press */
            done = true;
            break;
        }
        
        free(event);
    }

    /* close connection to server */
    xcb_disconnect(connection);

    return 0;
}
