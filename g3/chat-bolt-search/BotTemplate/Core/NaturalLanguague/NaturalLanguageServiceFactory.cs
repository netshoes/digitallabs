using System.Linq;
using System.Configuration;
using System.Collections.Generic;

namespace BotTemplate.Core.NaturalLanguage
{
    public class NaturalLanguageServiceFactory : INaturalLanguageServiceFactory
    {
        string NaturalLanguageService => ConfigurationManager.AppSettings["NaturalLanguageService"];

        IEnumerable<INaturalLanguageService> _NLServices;

        public NaturalLanguageServiceFactory(IEnumerable<INaturalLanguageService> NLServices)
        {
            _NLServices = NLServices;
        }

        public INaturalLanguageService Create()
        {
            return _NLServices.First(x => x.CanHandle(NaturalLanguageService));
        }
    }
}