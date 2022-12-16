#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

// taken from snake game for testing.
const NotificationSequence sequence_lose = {
    &message_vibro_on,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_note_ds4,
    &message_delay_10,
    &message_sound_off,
    &message_delay_10,

    &message_vibro_off,
    NULL,
};

const NotificationSequence sequence_win = {
    &message_note_c7,
    &message_delay_50,
    &message_sound_off,
    NULL,
};

// Handling Events
typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} ApplicationEvent;

// Types
typedef enum {
    None,
    Computer,
    Human,
    Draw,
} PlayerType;

typedef enum {
    X = 1,
    Y,
    DrawMove,
} MoveType;

typedef struct {
    uint8_t x;
    uint8_t y;
} SelectedTile;

typedef struct {
    uint8_t score;                      // keep track of the score
    MoveType grid [3][3];               // keep track of board.
    PlayerType currentTurn;         // keep track of whos turn it is
    SelectedTile selectedTile;      // keep track of highlighted tile 
    PlayerType winner;
    bool gameOver;
} GlobalAppState;

static void init_app_state(GlobalAppState* app_state){
    app_state->gameOver = false;
    app_state->score = 0;
    // randomize who goes first in future
    app_state->currentTurn = Human;
    // inital tile is 0,0
    SelectedTile initial_tile = {
        .x = 1,
        .y = 1,
    };
    app_state->selectedTile = initial_tile;
    // randomize who gets X in future 
    /*Player human = {
        .type_move = X,
        //.choseMove = false,
    };*/
    // init grid with empty spaces
    for (uint8_t x = 0; x < 3; x++) {
        for (uint8_t y = 0; y < 3; y++){
            app_state->grid[x][y] = 0;
        }
    }
    app_state->winner = None;
    //app_state->human = human;
}

static void draw_selected_square(Canvas* const canvas, uint8_t x, uint8_t y, const char* item){
    // hardcoded grid locations
    // find where to draw grid
    // if in first column
    if (x == 0 && y == 0) {
        // 0,0 
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 41, 24, item);
        return;
    } else if (x == 0 && y == 1) {
        // second column
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 41, 35, item);
        return;
    } else if (x == 0 && y == 2) {
        // second column
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 41, 46, item);
        return;
    } else if (x == 1 && y == 0) {
        // 0,0 
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 54, 24, item);
        return;
    } else if (x == 1 && y == 1) {
        // second column
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 54, 35, item);
        return;
    } else if (x == 1 && y == 2) {
        // second column
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 54, 46, item);
        return;
    } else if (x == 2 && y == 0) {
        // 0,0 
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 68, 24, item);
        return;
    } else if (x == 2 && y == 1) {
        // second column
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 68, 35, item);
        return;
    } else if (x == 2 && y == 2) {
        // second column
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 68, 46, item);
        return;
    } else {
        return;
    }
}

// Drawing Functions Start
static void draw_board (Canvas* const canvas, const GlobalAppState* state){
    canvas_draw_frame(canvas, 0, 0, 127, 63);
    canvas_set_font(canvas, FontPrimary);
    char buffer[12];
    snprintf(buffer,sizeof(buffer),"Score: %d",state->score);    
    canvas_draw_str(canvas, 4, 12, buffer);
    canvas_draw_line(canvas, 50, 15, 50, 45);
    canvas_draw_line(canvas, 65, 15, 65, 45);
    canvas_draw_line(canvas, 40, 25, 75, 25);
    canvas_draw_line(canvas, 40, 36, 75, 36);
    // here we draw whats been selected already.
    for (uint8_t x = 0; x < 3; x++) {
        for (uint8_t y = 0; y < 3; y++){
            if (state->grid[x][y] != 0) {
                state->grid[x][y] == X  ? draw_selected_square(canvas,x,y,"x") : draw_selected_square(canvas,x,y ,"o");
            }
        }
    }
}

static void clear_grid (GlobalAppState* app_state){
    for (uint8_t x = 0; x < 3; x++) {
        for (uint8_t y = 0; y < 3; y++){
            app_state->grid[x][y] = 0;
        }
    }
}


// Drawing Functions End



static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    // asserts event queue is allocated?
    furi_assert(event_queue);
    // create event from event
    // since we know this is an input callback we can set the event type to key
    // and dereference the actual input.
    ApplicationEvent event = {
        .type = EventTypeKey,
        .input = *input_event,
    };
    // we put the message on the event queue.
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
    //
}



static MoveType check_winner(const GlobalAppState* app_state) {
    // manually check for a winner
    // four outside walls
    // 0,0 -> 2,0
    if (app_state->grid[0][0] == app_state->grid[1][0] && app_state->grid[0][0] == app_state->grid[2][0] && app_state->grid[0][0] != 0){
        // someone won here return the move type of who won
        return app_state->grid[0][0];
    }
    // 0,0 -> 0,2
    if (app_state->grid[0][0] == app_state->grid[0][1] &&  app_state->grid[0][0] == app_state->grid[0][2] && app_state->grid[0][0] != 0){
        // someone won here return the move type of who won
        return app_state->grid[0][0];
    }
    // 0,2 -> 2,2
    if (app_state->grid[0][2] == app_state->grid[1][2] &&  app_state->grid[0][2] == app_state->grid[2][2] && app_state->grid[0][2] != 0){
        // someone won here return the move type of who won
        return app_state->grid[0][2];
    } 
    // 2,0 -> 2,2
    if (app_state->grid[2][0] == app_state->grid[2][1] && app_state->grid[2][0] == app_state->grid[2][2] && app_state->grid[2][0] != 0){
        // someone won here return the move type of who won
        return app_state->grid[2][0];
    } 
    // middle col 
    if (app_state->grid[1][0] == app_state->grid[1][1] && app_state->grid[1][0] ==  app_state->grid[1][2] && app_state->grid[1][0] != 0){
        // someone won here return the move type of who won
        return app_state->grid[1][0];
    }
    // middle row
    if (app_state->grid[0][1] == app_state->grid[1][1] && app_state->grid[0][1] ==  app_state->grid[2][1] && app_state->grid[0][1] != 0){
        // someone won here return the move type of who won
        return app_state->grid[0][1];
    }
    // two diagonal              
    // 0,0 -> 1,1 -> 2,2
    if (app_state->grid[0][0] == app_state->grid[1][1] && app_state->grid[0][0] ==  app_state->grid[2][2] && app_state->grid[0][0] != 0){
        // someone won here return the move type of who won
        return app_state->grid[0][0];
    }           
    // two diagonal              
    // 0,0 -> 1,1 -> 2,2
    if (app_state->grid[0][2] == app_state->grid[1][1] && app_state->grid[0][2] == app_state->grid[2][0] && app_state->grid[0][2] != 0){
        // someone won here return the move type of who won
        return app_state->grid[0][2];
    }        
    uint8_t count = 0;
    for (uint8_t x = 0; x < 3; x++) {
        for (uint8_t y = 0; y < 3; y++){
            if (app_state->grid[x][y] == 0) {
                count++;
            }
        }
    }
    if (count == 0){
        // no moves left its a draw -> 3
        return DrawMove;
    }
    return 0;     
}

static PlayerType set_winner(uint8_t winningMove) {
    switch (winningMove){
        case DrawMove:
            return Draw;
        case X:
            return Human;
        case Y:
            return Computer;
        default:
            return None; // no winner yet
    }
}


static void computer_move(GlobalAppState* app_state){
    // find unused squares and randomly pick one to write in
    SelectedTile* unused = (SelectedTile*)malloc(sizeof(SelectedTile) * 9); // only 9 possible moves? 
    uint8_t count = 0;
    for (uint8_t x = 0; x < 3; x++) {
        for (uint8_t y = 0; y < 3; y++){
            if (app_state->grid[x][y] == 0) {
                SelectedTile t = {
                    .x = x,
                    .y = y,
                };
                unused[count] = t; // this is a copy to the heap i hope
                count++;
            }
        }
    }
    uint8_t chosenTile = rand() % count;
    uint8_t computer_x = unused[chosenTile].x;
    uint8_t computer_y = unused[chosenTile].y;
    app_state->grid[computer_x][computer_y] = Y;
    free(unused);
    return; 
}

static bool handle_user_input(ApplicationEvent* event, GlobalAppState* app_state, NotificationApp* notification){
    // this should proably be reset in the render callback to avoid issues.
    if (app_state->winner != None){
        if (app_state->winner == Human){
            notification_message(notification,&sequence_win);
            app_state->score++;
        }
        if (app_state->winner == Computer && app_state->score > 0){
            notification_message_block(notification,&sequence_lose);            
            app_state->gameOver = true;
        }
        clear_grid(app_state);
        app_state->winner = None;
        return true;
    }
    // if its the computers turn the handle input.         
    if (app_state->currentTurn != Human) {   
        // do a computer move then set the turn back to human
        computer_move(app_state);
        app_state->currentTurn = Human;  
        uint8_t winner = check_winner(app_state);
        app_state->winner = set_winner(winner);
        return true;        
    }
    if (event->type == EventTypeKey){
        if (event->input.type == InputTypePress){
            switch(event->input.key){
            case InputKeyUp:
                if (app_state->selectedTile.y > 0)
                    app_state->selectedTile.y--; // move down by one                            
                break;
            case InputKeyDown:
                if (app_state->selectedTile.y < 2)
                    app_state->selectedTile.y++; // move up by one   
                break;
            case InputKeyRight:
                if (app_state->selectedTile.x < 2)
                    app_state->selectedTile.x++; // move right by one            
                break;
            case InputKeyLeft:
                if (app_state->selectedTile.x > 0)
                    app_state->selectedTile.x--; // move left by one                   
                break;
            case InputKeyOk:
                if(app_state->gameOver) {
                    app_state->gameOver = false;
                }            
                // if the spot is empty allow us to use it.
                if (app_state->grid[app_state->selectedTile.x][app_state->selectedTile.y] == 0) {
                    app_state->grid[app_state->selectedTile.x][app_state->selectedTile.y] = X;       
                    // check if we won
                    uint8_t winner = check_winner(app_state);
                    app_state->winner = set_winner(winner);
                    // set the turn to computer
                    app_state->currentTurn = Computer;
                    return true;
                }
                break;
            case InputKeyMAX: 
                break;
            case InputKeyBack: 
                // Exit the plugin
                return false;
            }
        } return true;
    }
    return true;
}

// Screen is 128x64 px
static void render_callback(Canvas* const canvas, void* ctx) {
    // acquire mutex on app_state
    const GlobalAppState* app_state = acquire_mutex((ValueMutex*)ctx,25);
    if (app_state == NULL)
        return;
    // clear canvas
    canvas_clear(canvas);
    // draw board
    draw_board(canvas,app_state);
    //char buffer[20];
    //snprintf(buffer,sizeof(buffer),"x %d y %d", app_state->selectedTile.x, app_state->selectedTile.y);
    //canvas_draw_str(canvas, 20, 60, buffer);
    // check whos currently playing if its a human trigger the highlighting etc.
        // Game Over banner
    if(app_state->gameOver) {
        // Screen is 128x64 px
        canvas_clear(canvas);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 34, 20, 62, 24);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 34, 20, 62, 24);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 37, 31, "Game Over");

        canvas_set_font(canvas, FontSecondary);
        char buffer[12];
        snprintf(buffer, sizeof(buffer), "Score: %d", app_state->score);
        canvas_draw_str_aligned(canvas, 64, 41, AlignCenter, AlignBottom, buffer);
        release_mutex((ValueMutex*)ctx,app_state);
        return;
    }
    if (app_state->currentTurn == Human){
        // draw X in box where user is
        uint8_t x = app_state->selectedTile.x;
        uint8_t y = app_state->selectedTile.y;
        draw_selected_square(canvas,x,y,"x");
    }
    release_mutex((ValueMutex*)ctx,app_state);
}


int32_t tic_tac_toe_app(){
    // create plugin state struct 
    GlobalAppState* app_state = malloc(sizeof(GlobalAppState));
    // init app state
    init_app_state(app_state);
    // create mutex to lock state
    ValueMutex state_mutex;
    if (!init_mutex(&state_mutex,app_state,sizeof(GlobalAppState))){
        FURI_LOG_E("Air Quality","Failed to create mutex\r\n");
        free(app_state);
        return 255;
    }
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(ApplicationEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_input_callback_set(view_port,input_callback,event_queue);
    view_port_draw_callback_set(view_port,render_callback,&state_mutex); // so mutex is passed to render callback
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui,view_port,GuiLayerFullscreen);
    ApplicationEvent event;
    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    for (bool processing = true; processing;){
        FuriStatus event_status = furi_message_queue_get(event_queue,&event,100);
        app_state = (GlobalAppState*)acquire_mutex_block(&state_mutex);
        if (event_status == FuriStatusOk){
            if (!handle_user_input(&event,app_state,notification))
            processing = false; // breaks
        } else {
            FURI_LOG_D("TicTacToe","FuriMessageQueue: event timeout");
        }
        view_port_update(view_port);
        release_mutex(&state_mutex,app_state);
    }
    view_port_enabled_set(view_port,false);
    gui_remove_view_port(gui,view_port);
    furi_record_close("gui");
    furi_record_close(RECORD_NOTIFICATION);
    view_port_free(view_port);
    furi_message_queue_free(event_queue); 
    return 0;
}

