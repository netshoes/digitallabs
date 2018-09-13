using System;
using System.Threading.Tasks;

namespace BotTemplate.Core.NaturalLanguage.Services
{
    [Serializable]
    public class WitNaturalLanguageService : INaturalLanguageService
    {
        public bool CanHandle(string naturalLangueService)
        {
            return naturalLangueService.ToLower() == "wit";
        }

        public Task<NaturalLanguageResult> Query(string queryString)
        {
            throw new NotImplementedException();
        }
    }
}
