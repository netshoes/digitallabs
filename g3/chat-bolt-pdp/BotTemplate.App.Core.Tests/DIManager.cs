using Autofac;
using System.Reflection;
using BotTemplate.Core.Dialogs;
using System.Collections.Generic;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.App.Core.Tests
{
    internal static class DIManager
    {
        public static IEnumerable<IIntentMapper> IntentMappers
        {
            get { return AutofacContainer.Resolve<IEnumerable<IIntentMapper>>(); }
        }

        public static DialogIntentMapper DialogIntentMapper
        {
            get { return AutofacContainer.Resolve<DialogIntentMapper>(); }

        }

        public static IEnumerable<INaturalLanguageService> NaturalLanguageServices
        {
            get { return AutofacContainer.Resolve<IEnumerable<INaturalLanguageService>>(); }

        }

        static IContainer _autofacContainer;

        public static IContainer AutofacContainer
        {
            get
            {
                if (_autofacContainer == null)

                {
                    var Builder = new ContainerBuilder();
                    var assembly = Assembly.Load("BotTemplate");

                    Builder.RegisterType<DialogIntentMapper>();

                    Builder.RegisterAssemblyTypes(assembly).AssignableTo<IIntentMapper>().AsImplementedInterfaces();
                    Builder.RegisterAssemblyTypes(assembly).AssignableTo<INaturalLanguageService>().AsImplementedInterfaces();
                    Builder.RegisterAssemblyTypes(assembly).AssignableTo<IRootDialog>().AsImplementedInterfaces();

                    _autofacContainer = Builder.Build();
                }
                return _autofacContainer;
            }
        }
    }
}
