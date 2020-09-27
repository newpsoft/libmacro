#include "tcreate.h"

#include "mcr/util/c11threads.h"

#include <QRandomGenerator>

void TCreate::base()
{
	mcr_context ctx;
	size_t bytesLeft;
	quint32 randBytes;
	char *iterBytes;
	for (size_t i = 0; i < COUNT; i++) {
		/* Start with random bytes */
		iterBytes = reinterpret_cast<char *>(&ctx);
		bytesLeft = sizeof(mcr_context);
		while (bytesLeft) {
			randBytes = QRandomGenerator::global()->generate();
			if (bytesLeft < sizeof(quint32)) {
				memcpy(iterBytes, &randBytes, bytesLeft);
				bytesLeft = 0;
			} else {
				memcpy(iterBytes, &randBytes, sizeof(randBytes));
				iterBytes += sizeof(randBytes);
				bytesLeft -= sizeof(randBytes);
			}
		}
		QCOMPARE(mcr_initialize(&ctx), 0);

		verifyContext(ctx);

		QCOMPARE(mcr_deinitialize(&ctx), 0);
	}
}

void TCreate::baseHeap()
{
	mcr_context *ctx;
	for (size_t i = 0; i < COUNT; i++) {
		ctx = mcr_allocate();
		QVERIFY(!!ctx);
		QCOMPARE(mcr_err, 0);

		verifyContext(*ctx);

		mcr_deallocate(ctx);
		QCOMPARE(mcr_err, 0);
	}
}

void TCreate::wholeContext()
{
	for (size_t i = 0; i < COUNT; i++) {
		mcr::Libmacro context(false);
	}
}

void TCreate::wholeContextHeap()
{
	mcr::Libmacro *context;
	for (size_t i = 0; i < COUNT; i++) {
		context = new mcr::Libmacro(false);
		delete context;
	}
}

void TCreate::verifyContext(struct mcr_context &ctx)
{
	int thrdErr;
	QCOMPARE(ctx.modifiers, 0);
	QCOMPARE(ctx.base.isignal_registry_pt, nullptr);
	QCOMPARE(ctx.base.itrigger_registry_pt, nullptr);
	thrdErr = mtx_lock(&ctx.base.dispatch_lock);
	QCOMPARE(thrdErr, thrd_success);
	mtx_unlock(&ctx.base.dispatch_lock);
	QCOMPARE(ctx.base.dispatchers, nullptr);
	QCOMPARE(ctx.base.dispatcher_count, 0);
	QCOMPARE(ctx.base.generic_dispatcher_flag, true);
	QCOMPARE(ctx.base.generic_dispatcher_pt, nullptr);
	QCOMPARE((void *)ctx.standard.iaction.receive, (void *)&mcr_Action_receive);
	QCOMPARE((void *)ctx.standard.ihid_echo.send, (void *)&mcr_HidEcho_send);
	QCOMPARE((void *)ctx.standard.ikey.send, (void *)&mcr_Key_send);
	QCOMPARE((void *)ctx.standard.imodifier.send, (void *)&mcr_Modifier_send);
	QCOMPARE((void *)ctx.standard.imove_cursor.send, (void *)&mcr_MoveCursor_send);
	QCOMPARE((void *)ctx.standard.inoop.send, (void *)&mcr_NoOp_send);
	QCOMPARE((void *)ctx.standard.iscroll.send, (void *)&mcr_Scroll_send);
	QCOMPARE(ctx.standard.key_modifiers, mcr_key_modifier_defaults);
	QCOMPARE(ctx.standard.key_modifier_count, mcr_key_modifier_default_count);
	QCOMPARE(ctx.standard.modifier_keys, mcr_modifier_key_defaults);
	QCOMPARE(ctx.standard.modifier_key_count, mcr_modifier_key_default_count);
	QVERIFY(!!ctx.standard.platform_pt);
	QCOMPARE(ctx.intercept.blockable, false);
	QVERIFY(!!ctx.intercept.platform_pt);
}

void TCreate::verifyContext(mcr::Libmacro &ctx)
{

}
