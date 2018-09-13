using System;
using Autofac;
using System.Reflection;
using System.Configuration;
using Microsoft.Bot.Connector;
using BotTemplate.Core.Dialogs;
using BotTemplate.Core.QnAMaker;
using Microsoft.Bot.Builder.Azure;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;
using BotTemplate.Core.Azure.Repository;
using Microsoft.Bot.Builder.Dialogs.Internals;
using BotTemplate.Integration;

namespace BotTemplate.Core.DI
{
    internal class DependencyInjector : IDependencyInjector
    {
        Assembly MainAssembly;

        public DependencyInjector()
        {
            MainAssembly = Assembly.GetExecutingAssembly();
        }

        public IDependencyInjector RegisterAzureStoreTable(ContainerBuilder Builder)
        {
            //Builder.RegisterModule(new AzureModule(Assembly.GetExecutingAssembly()));
            Builder.RegisterModule(new AzureModule(MainAssembly));
            
            var store = new TableBotDataStore(ConfigurationManager.AppSettings["AzureWebJobsStorage"]);

            //var store = new InMemoryDataStore();
            Builder.Register(c => store)
                .Keyed<IBotDataStore<BotData>>(AzureModule.Key_DataStore)
                .AsSelf()
                .SingleInstance();

            return this;
        }

        public IDependencyInjector RegisterNaturalLanguageServices(ContainerBuilder Builder)
        {
            Builder.RegisterType<NaturalLanguageServiceFactory>().As<INaturalLanguageServiceFactory>();

            Builder
            .RegisterAssemblyTypes(MainAssembly)
            .AssignableTo<INaturalLanguageService>()
            .AsImplementedInterfaces();

            return this;
        }

        public IDependencyInjector RegisterCustomerStore(ContainerBuilder Builder)
        {
            Builder.RegisterType<CustomerStore>().As<CustomerStore>();
                        
            return this;
        }

        public IDependencyInjector RegisterRootDialog(ContainerBuilder Builder)
        {
            Builder
            .RegisterAssemblyTypes(MainAssembly)
            .AssignableTo<IRootDialog>()
            .AsImplementedInterfaces();

            return this;
        }

        public IDependencyInjector RegisterIntentMappers(ContainerBuilder Builder)
        {
            Builder.RegisterType<DialogIntentMapper>();

            Builder
            .RegisterAssemblyTypes(MainAssembly)
            .AssignableTo<IIntentMapper>()
            .AsImplementedInterfaces();

            return this;
        }

        //public IDependencyInjector RegisterActivityLogger(ContainerBuilder Builder)
        //{
        //    Builder.RegisterType<LogRepository>().As<ILogRepository>();

        //    Builder
        //    .RegisterAssemblyTypes(MainAssembly)
        //    .AssignableTo<ILogRepository>()
        //    .AsImplementedInterfaces();

        //    return this;
        //}

        public IDependencyInjector RegisterQnA(ContainerBuilder Builder)
        {
            var key = ConfigurationManager.AppSettings["QnaAuthorization"];
            Builder.RegisterType<QnAMakerService>().As<IQnAMakerService>();

            return this;
        }

    }
}