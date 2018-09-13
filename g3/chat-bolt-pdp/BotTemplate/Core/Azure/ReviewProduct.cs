using System.Collections.Generic;
using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;

namespace BotTemplate.Core.Azure
{
    [BsonIgnoreExtraElements]
    public class ReviewProduct
    {
        [BsonElement("nome")]
        public string Nome { get; set; }

        [BsonElement("data")]
        public string Data { get; set; }

        [BsonElement("comment")]
        public string Comment { get; set; }

        [BsonElement("pros")]
        public IEnumerable<string> Pros { get; set; }

        [BsonElement("contras")]
        public IEnumerable<string> Contras { get; set; }

        public string GetFull()
        {
            return $"Review: {Comment}\n{GetList("Prós", Pros)}.\n{GetList("Contras", Contras)}";
        }

        private static string GetList(string title, IEnumerable<string> list)
        {
            return $"{title}: {string.Join(",", list)}";
        }
    }
}