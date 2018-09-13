using System.Web.Http;
using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.DI;

namespace BotTemplate
{
    public class WebApiApplication : System.Web.HttpApplication
    {
        //IDependencyInjector Injector;
        IDependencyInjector Injector;
        protected void Application_Start()
        {
            Injector = new DependencyInjector();
            Conversation.UpdateContainer(
                builder =>
                {
                    Injector
                    .RegisterAzureStoreTable(builder)
                    .RegisterNaturalLanguageServices(builder)
                    .RegisterRootDialog(builder)
                    .RegisterIntentMappers(builder)
                    .RegisterCustomerStore(builder)
                    .RegisterQnA(builder);
                });
            GlobalConfiguration.Configure(WebApiConfig.Register);
        }
    }
}
