using System.Threading.Tasks;

namespace BotTemplate.Core.NaturalLanguage
{
    public interface INaturalLanguageService
    {
        Task<NaturalLanguageResult> Query(string queryString);

        bool CanHandle(string naturalLangueService);
    }
}
