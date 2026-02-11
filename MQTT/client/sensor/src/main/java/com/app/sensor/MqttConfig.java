package com.app.sensor;
import com.hivemq.client.mqtt.MqttClient;
import com.hivemq.client.mqtt.mqtt5.Mqtt5AsyncClient;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import java.util.UUID;

@Configuration
public class MqttConfig {

    @Value("${hivemq.broker.host}")
    private String brokerHost;

    @Value("${hivemq.broker.port}")
    private int brokerPort;

    @Value("${hivemq.broker.username}")
    private String username;

    @Value("${hivemq.broker.password}")
    private String password;

    // create an MQTT client
    @Bean
    public Mqtt5AsyncClient mqtt5Client() {
        Mqtt5AsyncClient client = MqttClient.builder()
                .useMqttVersion5()
                .identifier(UUID.randomUUID().toString()) // Unique client ID
                .serverHost(brokerHost)
                .serverPort(brokerPort)
                .sslWithDefaultConfig() // Enable TLS for secure connection to HiveMQ Cloud
                .buildAsync();
        
        // Connect to HiveMQ Cloud with TLS and username/pw
        client.connectWith()
                .simpleAuth()
                    .username(username)
                    .password(password.getBytes())
                    .applySimpleAuth()
                .send()
                .whenComplete((connAck, throwable) -> {
                    if (throwable != null) {
                        System.err.println("Connection failed: " + throwable.getMessage());
                    } else {
                        System.out.println("Successfully connected to HiveMQ broker!");
                    }
                });

        return client;
    }
}
