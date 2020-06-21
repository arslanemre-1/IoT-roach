using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using MQTTnet.Extensions.ManagedClient;
using Roach.Models;
using Roach.Services;

namespace Roach.Controllers
{
    public class HomeController : Controller
    {
//        private MqttService MqttService { get; set; }
//
//        public HomeController()
//        {
//            MqttService = new MqttService();
//        }

        public async Task<IActionResult> Index()
        {
            // send mqttt message (request info)
            // receive answer -> store in viewmodel

//            await MqttService.ConnectAsync();
//            await MqttService.Client.UseApplicationMessageReceivedHandler(e =>
//            {
//                try
//                {
//                    string topic = e.ApplicationMessage.Topic;
//                    if (string.IsNullOrWhiteSpace(topic) == false)
//                    {
//                        string payload = Encoding.UTF8.GetString(e.ApplicationMessage.Payload);
//                        Console.WriteLine($"Topic: {topic}. Message Received: {payload}");
//                    }
//                }
//                catch (Exception ex)
//                {
//                    Console.WriteLine(ex.Message, ex);
//                }
//            });
            
            
            return View(/*viewmodel*/);
        }

        public IActionResult Privacy()
        {
            return View();
        }

        [ResponseCache(Duration = 0, Location = ResponseCacheLocation.None, NoStore = true)]
        public IActionResult Error()
        {
            return View(new ErrorViewModel {RequestId = Activity.Current?.Id ?? HttpContext.TraceIdentifier});
        }
    }
}