import pika
import json


class RemoteQueue:
    @staticmethod
    def publish_workflows_to_queue(workflows):
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        channel = connection.channel()

        try:
            for workflow in workflows:
                message_body = str(workflow.to_service_id) + "\t" \
                               + str(workflow.to_service_number) + "\t" \
                               + str(workflow.to_value) + "\n"

                message = {'address': workflow.to_peripheral.address, 'body': message_body}

                # Declare the exchange
                channel.exchange_declare(exchange='base-ic.exchange',
                                         type='direct',
                                         durable=True)

                # Declare the queue
                channel.queue_declare(queue='base-ic.' + workflow.to_peripheral.queue + '-worker',
                                      durable=True)

                # Bind the queue to the exchange with the correct routing key
                channel.queue_bind(queue='base-ic.' + workflow.to_peripheral.queue + '-worker',
                                   exchange='base-ic.exchange',
                                   routing_key='route.' + workflow.to_peripheral.queue + '-worker')

                # Publish the message to the queue
                channel.basic_publish(exchange='base-ic.exchange',
                                      routing_key='route.' + workflow.to_peripheral.queue + '-worker',
                                      body=json.dumps(message))

                print(" [x] Sent " + message_body)
        finally:
            connection.close()
