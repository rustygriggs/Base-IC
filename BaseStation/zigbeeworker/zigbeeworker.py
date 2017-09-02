"""
Example async consumer and publisher that will reconnect
automatically when a connection to rabbitmq is broken and
restored.
Note that no attempt is made to re-send messages that are
generated while the connection is down.

http://asynqp.readthedocs.io/en/v0.4/examples.html#reconnecting
"""
import asyncio
import asynqp
from asyncio.futures import InvalidStateError
from zigbeeserial import ZigbeeSerial
from queue import Queue

# Global variables are ugly, but this is a simple example
CHANNELS = []
CONNECTION = None
CONSUMER = None
QUEUEMONITOR = None


async def setup_connection():
    # connect to the RabbitMQ broker
    connection = await asynqp.connect('localhost', 5672, username='guest', password='guest')
    return connection


async def setup_exchange_and_queue(connection):
    # Open a communications channel
    channel = await connection.open_channel()

    # Create a queue and an exchange on the broker
    exchange = await channel.declare_exchange('base-ic.exchange', 'direct')
    zigbee_queue = await channel.declare_queue('base-ic.zigbee-worker')
    api_worker_queue = await channel.declare_queue('base-ic.api-worker')

    # Save a reference to each channel so we can close it later
    CHANNELS.append(channel)

    # Bind the queue to the exchange, so the queue will get messages published to the exchange
    await zigbee_queue.bind(exchange, 'route.zigbee-worker')
    await api_worker_queue.bind(exchange, 'route.api-worker')

    return exchange, zigbee_queue, api_worker_queue


async def setup_consumer(connection):
    # callback will be called each time a message is received from the queue
    def callback(msg):
        print('Received: {}'.format(msg.body))
        msg.ack()

    _, zigbee_queue, _ = await setup_exchange_and_queue(connection)

    # connect the callback to the queue
    consumer = await zigbee_queue.consume(callback)
    return consumer


async def start(loop):
    """
    Creates a connection, starts the consumer and producer.
    If it fails, it will attempt to reconnect after waiting
    1 second
    """
    global CONNECTION
    global CONSUMER
    global QUEUEMONITOR

    try:
        CONNECTION = await setup_connection()
        CONSUMER = await setup_consumer(CONNECTION)
        QUEUEMONITOR = loop.create_task(monitor_queue(CONNECTION))
    # Multiple exceptions may be thrown, ConnectionError, OsError
    except Exception:
        print('failed to connect, trying again.')
        await asyncio.sleep(1)
        loop.create_task(start(loop))


async def stop():
    """
    Cleans up connections, channels, consumers and producers
    when the connection is closed.
    """
    global CHANNELS
    global CONNECTION
    global CONSUMER

    await CONSUMER.cancel()  # this is a coroutine

    for channel in CHANNELS:
        await channel.close()
    CHANNELS = []

    if CONNECTION is not None:
        try:
            await CONNECTION.close()
        except InvalidStateError:
            pass  # could be automatically closed, so this is expected
        CONNECTION = None


async def monitor_queue(connection):
    exchange, zigbee_queue, api_worker_queue = await setup_exchange_and_queue(connection)

    while True:
        print("Queue length: {}".format(rx_queue.qsize()))

        # TODO: if a queue length is detected then we received something from the ZigBee
        # and we need to send that message to RabbitMQ and remove it from the queue.

        if not rx_queue.empty():
            # print(rxQueue.get_nowait())
            msg = asynqp.Message(rx_queue.get())
            exchange.publish(msg, 'route.api-worker')

        await asyncio.sleep(0.25)


def connection_lost_handler(loop, context):
    """
    Here we setup a custom exception handler to listen for
    ConnectionErrors.

    The exceptions we can catch follow this inheritance scheme

        - ConnectionError - base
            |
            - asynqp.exceptions.ConnectionClosedError - connection closed properly
                |
                - asynqp.exceptions.ConnectionLostError - closed unexpectedly
    """
    exception = context.get('exception')
    if isinstance(exception, asynqp.exceptions.ConnectionClosed):
        print('Connection lost -- trying to reconnect')
        # close everything before reconnecting
        close_task = loop.create_task(stop())
        asyncio.wait_for(close_task, None)
        # reconnect
        loop.create_task(start(loop))
    else:
        # default behaviour
        loop.default_exception_handler(context)


if __name__ == '__main__':
    # The rx queue will be for any thing the serial device has received.
    rx_queue = Queue()
    # The tx queue will be for any thing that needs to be sent over the serial device.
    tx_queue = Queue()

    xbee = ZigbeeSerial(rx_queue, tx_queue, port="/dev/tty.usbserial-A603U21W")

    task_loop = asyncio.get_event_loop()
    task_loop.set_exception_handler(connection_lost_handler)
    task_loop.create_task(start(task_loop))
    task_loop.run_forever()
