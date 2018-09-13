using System;

namespace BotTemplate.Core.NaturalLanguage
{
    [Serializable]
    public class NaturalLanguageIntent
    {
        public string Intent { get; set; }
        public double? Score { get; set; }
    }
}