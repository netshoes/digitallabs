using System;
using System.Threading;
using System.Configuration;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Luis;
using System.Collections.Generic;

namespace BotTemplate.Core.NaturalLanguage.Services
{
    [Serializable]
    public class LuisNaturalLanguageService : INaturalLanguageService
    {
        ILuisService _MSLuisService;

        string ModelID => ConfigurationManager.AppSettings["LuisModelID"];
        string SubscriptionKey => ConfigurationManager.AppSettings["LuisSubscriptionKey"];
        string Region => ConfigurationManager.AppSettings["LuisRegion"];

        public LuisNaturalLanguageService()
        {
            _MSLuisService = new LuisService(new LuisModelAttribute(ModelID, SubscriptionKey, LuisApiVersion.V2, Region) { TimezoneOffset = -180, Verbose = true });
        }

        public async Task<NaturalLanguageResult> Query(string queryString)
        {
            var result = await _MSLuisService.QueryAsync(queryString, CancellationToken.None);
            var list = new NaturalLanguageResult()
            {
                Entities = new List<NaturalLanguageEntity>(),
                Intents = new List<NaturalLanguageIntent>()
            };

            foreach (var item in result.Intents)
                list.Intents.Add(new NaturalLanguageIntent()
                {
                    Intent = item.Intent,
                    Score = item.Score
                });

            foreach (var item in result.Entities)
                list.Entities.Add(new NaturalLanguageEntity()
                {
                    EndIndex = item.EndIndex,
                    Entity = item.Entity,
                    Role = item.Role,
                    Score = item.Score,
                    StartIndex = item.StartIndex,
                    Type = item.Type
                });
            return list;
        }

        public bool CanHandle(string naturalLangueService)
        {
            return naturalLangueService.ToLower() == "luis";
        }
    }
}
