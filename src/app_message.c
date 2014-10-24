#include <pebble.h>

static Window *window;	
static TextLayer *question_layer;
static TextLayer *answer_layer;
static AppTimer *timer;
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1,
  REQUEST_KEY = 2,
};

typedef enum {
  NAME = 0,
  QUEST = 1,
  FAVORITE_COLOR = 2
} requests;

static uint8_t maxRequests = 3;
static requests currentRequest = NAME;

static char *questions[] = {
  "What is your name?",
  "What is your quest?",
  "What is your favorite colour?",
  "Right, off you go."
};

// Write message to buffer & send
void send_message(requests r){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
  dict_write_uint8(iter, REQUEST_KEY, r);
	
	dict_write_end(iter);
  	app_message_outbox_send();
}

static void timer_callback(void *data) {
  text_layer_set_text(question_layer, questions[currentRequest]);
  text_layer_set_text(answer_layer, "");
  if (currentRequest < maxRequests) {
    send_message(currentRequest);
    currentRequest++;    
  }
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {    
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
    text_layer_set_text(answer_layer, tuple->value->cstring);
    timer = app_timer_register(3000, timer_callback, NULL);
	}
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

// Handle the loading of window elements
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  question_layer = text_layer_create(GRect(0, 0, 144, 68));
  text_layer_set_text_color(question_layer, GColorWhite);
  text_layer_set_background_color(question_layer, GColorClear);
  text_layer_set_font(question_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(question_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(question_layer));

  answer_layer = text_layer_create(GRect(0, 70, 144, 68));
  text_layer_set_text_color(answer_layer, GColorWhite);
  text_layer_set_background_color(answer_layer, GColorClear);
  text_layer_set_font(answer_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(answer_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(answer_layer));
  
  timer = app_timer_register(500, timer_callback, NULL);
}

// Destroy window elements
static void window_unload(Window *window) {  
  text_layer_destroy(question_layer);
  text_layer_destroy(answer_layer);
}

void init(void) {
	window = window_create();
  window_set_background_color(window, GColorBlack);
  window_set_fullscreen(window, true);
    window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  window_stack_push(window, true);
}

void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}