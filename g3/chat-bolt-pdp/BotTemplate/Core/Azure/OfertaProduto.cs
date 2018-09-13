using MongoDB.Bson;
using MongoDB.Bson.Serialization.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace BotTemplate.Core.Azure
{
    public class OfertaProduto
    {
        [BsonId]
        public ObjectId Id { get; set; }

        [BsonElement("conversetionID")]
        public string ConversetionID { get; set; }

        [BsonElement("count")]
        public int Quantidade { get; set; }
    }
}