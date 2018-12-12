#include <xcb/xcb.h>
#include <cairo-xcb.h>
#include <stdio.h>
#include <stdlib.h>

xcb_visualtype_t* find_visual(xcb_connection_t* connection, xcb_visualid_t visual)
{
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(connection));

    for (; screen_iter.rem; xcb_screen_next(&screen_iter))
    {
        xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);
        for (; depth_iter.rem; xcb_depth_next(&depth_iter))
        {
            xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
            for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
            {
                if (visual == visual_iter.data->visual_id)
                    return visual_iter.data;
            }
        }
    }

    return NULL;
}

int main()
{
    xcb_connection_t* connection;
    xcb_screen_t* screen;
    xcb_window_t window;
    xcb_visualtype_t* visual;
    xcb_generic_event_t* event;
    cairo_surface_t* surface;
    uint32_t mask[2];
    cairo_t* cr;

    connection = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(connection))
    {
        fprintf(stderr, "Could not connect to X11 server");
        return 1;
    }

    mask[0] = 1;
    mask[1] = XCB_EVENT_MASK_EXPOSURE;
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    window = xcb_generate_id(connection);
    xcb_create_window(connection, XCB_COPY_FROM_PARENT, window, screen->root,
            20, 20, 150, 150, 1,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK,
            mask);

    xcb_map_window(connection, window);

    visual = find_visual(connection, screen->root_visual);
    if (visual == NULL)
    {
        fprintf(stderr, "Could not find visual");
        xcb_disconnect(connection);
        return 1;
    }

    surface = cairo_xcb_surface_create(connection, window, visual, 150, 150);
    cr = cairo_create(surface);
    
    xcb_flush(connection);
    
    while ((event = xcb_wait_for_event(connection)))
    {
        switch (event->response_type & ~0x80)
        {
            case XCB_EXPOSE:
                /* Avoid extra redraws by checking if this is
                 * the last expose event in the sequence
                 */
                if (((xcb_expose_event_t*) event)->count != 0)
                    break;

                cairo_set_source_rgb(cr, 0, 1, 0);
                cairo_paint(cr);

                cairo_set_source_rgb(cr, 1, 0, 0);
                cairo_move_to(cr, 0, 0);
                cairo_line_to(cr, 150, 0);
                cairo_line_to(cr, 150, 150);
                cairo_close_path(cr);
                cairo_fill(cr);

                cairo_set_source_rgb(cr, 0, 0, 1);
                cairo_set_line_width(cr, 20);
                cairo_move_to(cr, 0, 150);
                cairo_line_to(cr, 150, 0);
                cairo_stroke(cr);

                cairo_surface_flush(surface);
                break;
        }
        
        free(event);
        xcb_flush(connection);
    }

    
    cairo_surface_finish(surface);
    cairo_surface_destroy(surface);
    xcb_disconnect(connection);
 
    return 0;
}
