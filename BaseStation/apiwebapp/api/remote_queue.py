import pika
import json


class RemoteQueue:
    @staticmethod
    def publish_recipes_to_queue(recipes):
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        channel = connection.channel()

        try:
            for recipe in recipes:
                message_body = str(recipe.output_peripheral_service.service_id) + "\t" \
                               + str(recipe.output_peripheral_service.service_number) + "\t" \
                               + str(recipe.output_value) + "\n"

                message = {'address': recipe.output_peripheral_service.peripheral.address, 'body': message_body}

                # Declare the exchange
                channel.exchange_declare(exchange='base-ic.exchange',
                                         type='direct',
                                         durable=True)

                # Declare the queue
                channel.queue_declare(queue='base-ic.' + recipe.output_peripheral_service.peripheral.queue + '-worker',
                                      durable=True)

                # Bind the queue to the exchange with the correct routing key
                channel.queue_bind(queue='base-ic.' + recipe.output_peripheral_service.peripheral.queue + '-worker',
                                   exchange='base-ic.exchange',
                                   routing_key='route.' + recipe.output_peripheral_service.peripheral.queue + '-worker')

                # Publish the message to the queue
                channel.basic_publish(exchange='base-ic.exchange',
                                      routing_key='route.' + recipe.output_peripheral_service.peripheral.queue + '-worker',
                                      body=json.dumps(message))

                print(" [x] Sent " + message_body)
        finally:
            connection.close()
