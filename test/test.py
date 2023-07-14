import asyncio

async def test_main():
    lock = asyncio.Lock()
    def sync_func():
        with lock:
            print('sync_func')

    async def async_func():
        async with lock:
            await asyncio.sleep(1)
            print('async_func')

    t = asyncio.create_task(async_func())
    sync_func()
    await t


asyncio.run(test_main())
