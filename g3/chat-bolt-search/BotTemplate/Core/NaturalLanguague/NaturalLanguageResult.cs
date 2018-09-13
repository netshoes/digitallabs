using System;
using System.Collections.Generic;

namespace BotTemplate.Core.NaturalLanguage
{
    [Serializable]
    public class NaturalLanguageResult
    {
        public List<NaturalLanguageIntent> Intents { get; set; }
        public List<NaturalLanguageEntity> Entities { get; set; }
    }
}