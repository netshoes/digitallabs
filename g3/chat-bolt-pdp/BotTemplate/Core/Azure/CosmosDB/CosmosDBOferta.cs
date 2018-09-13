using MongoDB.Bson;
using MongoDB.Driver;
using System;
using System.Linq;
using System.Linq.Expressions;
using System.Threading.Tasks;

namespace BotTemplate.Core.Azure.CosmosDB
{
    public class CosmosDBOferta
    {
        IMongoClient Client;
        IMongoDatabase Database;
        IMongoCollection<OfertaProduto> Collection;

        public CosmosDBOferta()
        {
            var client = new MongoClient("mongodb://grupotres:m0Kn17k9EECoCJGkfKJrY5uXgJglUwffUOg30KBgkJBALQ3lKPUZ7aZ7r6vkmkbNKVGtx6xT1bAvhm7Gt01paA==@grupotres.documents.azure.com:10255/?ssl=true&replicaSet=globaldb");
            var db = client.GetDatabase("chatbolt");
            Collection = db.GetCollection<OfertaProduto>("ofertabolt");
        }

        public async Task<OfertaProduto> GetOfertaId(string conversetionID)
        {
            return Collection.Find<OfertaProduto>(x => x.ConversetionID == conversetionID).FirstOrDefault();
        }

        public async Task<OfertaProduto> IncrementaOferta(string conversetionID)
        {
            OfertaProduto off = null;
            try
            {
                off = await this.GetOfertaId(conversetionID);
                if (off.Quantidade == 2)
                {
                    var offUpdate = Builders<OfertaProduto>.Update
                        .Set(v => v.Quantidade, 0);
                    Collection.UpdateOne<OfertaProduto>(x => x.ConversetionID == conversetionID, offUpdate);
                    throw new Exception();
                }
                else
                {
                    var offUpdate = Builders<OfertaProduto>.Update
                        .Set(v => v.Quantidade, off.Quantidade + 1);
                    Collection.UpdateOne<OfertaProduto>(x => x.ConversetionID == conversetionID, offUpdate);
                }
            }
            catch(Exception e)
            {
                if(off != null)
                {
                    throw e;
                }
                off = new OfertaProduto();
                off.ConversetionID = conversetionID;
                off.Quantidade = 1;
                Collection.InsertOne(off);
            }
            
            return null;
        }
    }
}