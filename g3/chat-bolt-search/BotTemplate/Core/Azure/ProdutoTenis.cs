using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
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

        [BsonElement("code")]
        public string code { get; set; }

        [BsonElement("name")]
        public string name { get; set; }

        [BsonElement("genders")]
        public List<string> genders { get; set; }

        //[BsonElement("colorName")]
        //public string colorName { get; set; }


        [BsonElement("images")]
        public images images { get; set; }

    }

    [BsonIgnoreExtraElements]
    public class images
    {
        [BsonElement("image")]
        public string image { get; set; }
    }
}