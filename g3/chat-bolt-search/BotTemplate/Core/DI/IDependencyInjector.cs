using Autofac;

namespace BotTemplate.Core.DI
{
    public interface IDependencyInjector
    {
        IDependencyInjector RegisterRootDialog(ContainerBuilder Builder);
        IDependencyInjector RegisterIntentMappers(ContainerBuilder Builder);
        IDependencyInjector RegisterAzureStoreTable(ContainerBuilder Builder);
        IDependencyInjector RegisterNaturalLanguageServices(ContainerBuilder Builder);
        IDependencyInjector RegisterCustomerStore(ContainerBuilder Builder);
        IDependencyInjector RegisterQnA(ContainerBuilder Builder);
    }
}