using MongoDB.Bson;
using MongoDB.Driver;
using System.Linq;

namespace BotTemplate.Core.Azure
{
    public class CosmosDBProduto
    {
        IMongoClient Client;
        IMongoDatabase Database;
        IMongoCollection<ProdutoTenis> Collection;

        public CosmosDBProduto()
        {
            var client = new MongoClient("mongodb://grupotres:m0Kn17k9EECoCJGkfKJrY5uXgJglUwffUOg30KBgkJBALQ3lKPUZ7aZ7r6vkmkbNKVGtx6xT1bAvhm7Gt01paA==@grupotres.documents.azure.com:10255/?ssl=true&replicaSet=globaldb");
            var db = client.GetDatabase("chatbolt");
            Collection = db.GetCollection<ProdutoTenis>("chatbolt");
        }

        public ProdutoTenis GetProdutoId(string idProduto)
        {
            return Collection.Find(x => x.Id == ObjectId.Parse(idProduto)).FirstOrDefault();
        }
    }
}