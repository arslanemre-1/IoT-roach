using System;
using System.Threading.Tasks;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;
using MQTTnet.Extensions.ManagedClient;

namespace Roach.Services
{
    public class MqttService
    {
        public IManagedMqttClient Client { get; set; }
        
        public async Task ConnectAsync()
        {
            string clientId = Guid.NewGuid().ToString();
            string mqttURI = "farmer.cloudmqtt.com";
            string mqttUser = "tdawhnyd";
            string mqttPassword = "4r4Bks-G0vuE";
            int mqttPort = 22017;
            bool mqttSecure = true;
            var messageBuilder = new MqttClientOptionsBuilder()
                .WithClientId(clientId)
                .WithCredentials(mqttUser, mqttPassword)
                .WithTcpServer(mqttURI, mqttPort)
                .WithCleanSession();
            var options = mqttSecure
                ? messageBuilder
                    .WithTls()
                    .Build()
                : messageBuilder
                    .Build();
            var managedOptions = new ManagedMqttClientOptionsBuilder()
                .WithAutoReconnectDelay(TimeSpan.FromSeconds(5))
                .WithClientOptions(options)
                .Build();
            Client = new MqttFactory().CreateManagedMqttClient();
            await Client.StartAsync(managedOptions);
        }
        
        public async Task PublishAsync(string topic, byte[] payload, bool retainFlag = true, int qos = 1)
        {
            await Client.PublishAsync(new MqttApplicationMessageBuilder()
                .WithTopic(topic)
                .WithPayload(payload)
                .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel) qos)
                .WithRetainFlag(retainFlag)
                .Build());
        }

        public async Task SubscribeAsync(string topic, int qos = 1)
        {
            await Client.SubscribeAsync(new TopicFilterBuilder()
                .WithTopic(topic)
                .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel) qos)
                .Build());
        }
    }
}