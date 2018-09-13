using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Threading.Tasks;
using System.Web;
using MongoDB.Bson;
using MongoDB.Driver;

namespace BotTemplate.Core.Azure
{
    public class AzureCosmosDBClient
    {
        IMongoClient Client;
        IMongoDatabase Database;
        IMongoCollection<ProdutoTenis> Collection;

        public AzureCosmosDBClient()
        {
            var client = new MongoClient("mongodb://grupotres:m0Kn17k9EECoCJGkfKJrY5uXgJglUwffUOg30KBgkJBALQ3lKPUZ7aZ7r6vkmkbNKVGtx6xT1bAvhm7Gt01paA==@grupotres.documents.azure.com:10255/?ssl=true&replicaSet=globaldb");
            var db = client.GetDatabase("chatbolt");
            Collection = db.GetCollection<ProdutoTenis>("chatbolt");
        }

        public async Task<IEnumerable<ProdutoTenis>> GetProdutoByThings(string genero, string  pisada, string objetivo, string  produto, string cor, bool useTenis = false)
        {

            //var teste = Collection.Find(new BsonDocument()).FirstOrDefault();

            if (genero.Equals("homem"))
            {
                genero = "Masculino";
            }

            if (genero.Equals("mulher"))
            {
                genero = "Feminino";
            }

            
            var query = "{  ";
            if (pisada != null)
            {
                query += "\"pisada\" : \""+ pisada.ToLower()+"\", ";
            }

            if (genero != null)
            {
                query += " \"genders\": { \"$in\": [\"" + genero + "\"] }, ";
            }

            if (useTenis)
            {
                query += " \"productType\":\"Tênis\", ";

            }
            query += " }";

            var qq = BsonDocument.Parse(query);
            var result = await Collection.Find(qq).Limit(3).ToListAsync();

            return result;
        }
    }
}