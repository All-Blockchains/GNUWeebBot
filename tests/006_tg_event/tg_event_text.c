
#include <libgen.h>
#include <assert.h>
#include "006_tg_event.h"
#include <gwbot/lib/tg_event.h>




static int test_tg_event_text_001_simple(void)
{
	int ret;
	char *json_str;
	struct tgev evt;
	struct tgevi_entity *entity;
	struct tgev_text *msg_text;
	struct tgevi_from *from;
	struct tgevi_chat *chat;

	json_str = load_str_from_file("tg_event_text/001_simple.json");
	ret = tg_event_load_str(json_str, &evt);
	free(json_str);

	TQ_ASSERT((ret == 0), 10);
	TQ_ASSERT((evt.type == TGEV_TEXT), 10);
	TQ_ASSERT((evt.json != NULL), 10);
	TQ_ASSERT((evt.update_id == 346089057), 10);

	msg_text = &evt.msg_text;

	TQ_ASSERT((msg_text->msg_id == 525u), 10);
	TQ_ASSERT((msg_text->date == 1616588789ul), 10);
	TQ_ASSERT((!strcmp(msg_text->text, "/debug Hello World!")), 10);
	TQ_ASSERT((msg_text->entity_c == 1), 10);
	TQ_ASSERT((msg_text->entities != NULL), 10);

	entity = &msg_text->entities[0];
	TQ_ASSERT((entity->offset == 0), 10);
	TQ_ASSERT((entity->length == 6), 10);
	TQ_ASSERT((!strcmp(entity->type, "bot_command")), 10);
	TQ_ASSERT((msg_text->reply_to == NULL), 10);

	from = &msg_text->from;

	TQ_ASSERT((from->id == 243692601ul), 10);
	TQ_ASSERT((from->is_bot == false), 10);
	TQ_ASSERT((!strcmp(from->first_name, "cpu_relax();")), 10);
	TQ_ASSERT((!strcmp(from->last_name, "//")), 10);
	TQ_ASSERT((!strcmp(from->lang, "en")), 10);


	chat = &msg_text->chat;

	TQ_ASSERT((chat->id == -1001422514298l), 10);
	TQ_ASSERT((!strcmp(chat->title, "GNU/Weeb Test Driven Development")),
		  10);
	TQ_ASSERT((!strcmp(chat->username, "GNUWeebTDD")), 10);
	TQ_ASSERT((!strcmp(chat->type, "supergroup")), 10);

	tg_event_destroy(&evt);

	return 0;
}


struct list_func tg_event_text_list[] = {
	{test_tg_event_text_001_simple, 220},
	{NULL, 0}
};

int tg_event_text(void)
{
	size_t num = sizeof(tg_event_text_list) / sizeof(*tg_event_text_list);
	for (size_t i = 0; i < num; i++) {
		if (tg_event_text_list[i].func)
			tg_event_text_list[i].func();
	}

	return 0;
}
