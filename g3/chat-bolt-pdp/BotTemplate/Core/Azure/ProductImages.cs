using MongoDB.Bson.Serialization.Attributes;

namespace BotTemplate.Core.Azure
{
    [BsonIgnoreExtraElements]
    public class ProductImages
    {
        [BsonElement("image")]
        public string Image { get; set; }
    }
}