using System.Collections.Generic;
using System.Linq;
using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;

namespace BotTemplate.Core.Azure
{
    [BsonIgnoreExtraElements]
    public class ProdutoTenis
    {
        [BsonId]
        public ObjectId Id { get; set; }

        [BsonElement("pisada")]
        public string Pisada { get; set; }

        [BsonElement("name")]
        public string Name { get; set; }

        [BsonElement("code")]
        public string Code { get; set; }

        [BsonElement("reviews")]
        public IList<ReviewProduct> Reviews { get; set; }

        [BsonElement("reviewStars")]
        public decimal Stars { get; set; }

        [BsonElement("images")]
        public ProductImages Images { get; set; }

        public ReviewProduct BestReview => Reviews.FirstOrDefault();
    }
}