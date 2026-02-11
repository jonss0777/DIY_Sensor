package com.app.sensor;

import com.hivemq.client.mqtt.datatypes.MqttQos;
import com.hivemq.client.mqtt.mqtt5.Mqtt5AsyncClient;
import com.hivemq.client.mqtt.mqtt5.message.publish.Mqtt5Publish;
import jakarta.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.nio.charset.StandardCharsets;
import java.util.function.Consumer;

@Service
public class MqttSubscriberService {

    private final Mqtt5AsyncClient mqtt5AsyncClient;

    @Autowired
    public MqttSubscriberService(Mqtt5AsyncClient mqtt5AsyncClient) {
        this.mqtt5AsyncClient = mqtt5AsyncClient;
    }

    @PostConstruct
    public void subscribeToTopic() {
        String topic = "HTSensor"; 

        Consumer<Mqtt5Publish> consumer = publish -> {
            String message = new String(publish.getPayloadAsBytes(), StandardCharsets.UTF_8);
            System.out.println("Received message on topic '" + publish.getTopic() + "': " + message);
        };

        mqtt5AsyncClient.subscribeWith()
                .topicFilter(topic)
                .qos(MqttQos.AT_LEAST_ONCE)
                .callback(consumer)
                .send()
                .whenComplete((mqtt5SubAck, throwable) -> {
                    if (throwable != null) {
                        System.err.println("Subscription failed: " + throwable.getMessage());
                    } else {
                        System.out.println("Successfully subscribed to topic: " + topic);
                    }
                });
    }
}
