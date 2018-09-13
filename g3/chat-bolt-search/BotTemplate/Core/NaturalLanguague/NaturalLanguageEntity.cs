using System;

namespace BotTemplate.Core.NaturalLanguage
{
    [Serializable]
    public class NaturalLanguageEntity
    {
        public string Role { get; set; }
        public string Entity { get; set; }
        public string Type { get; set; }
        public int? StartIndex { get; set; }
        public int? EndIndex { get; set; }
        public double? Score { get; set; }
    }
}