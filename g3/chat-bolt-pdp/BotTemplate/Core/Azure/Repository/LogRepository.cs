using System;
using System.Configuration;
using System.Threading.Tasks;
using Microsoft.Bot.Connector;
using BotTemplate.Core.Azure.Entity;
using Microsoft.WindowsAzure.Storage;
using Microsoft.WindowsAzure.Storage.Table;

namespace BotTemplate.Core.Azure.Repository
{
    [Serializable]
    public class LogRepository : ILogRepository
    {
        public async Task LogAsync(IActivity activity)
        {
            var tableClient = GetTableClient();

            var table = tableClient.GetTableReference("botConversationLogs");

            LogEntity entity = new LogEntity(activity.AsMessageActivity());

            var tableOperation = TableOperation.Insert(entity);

            await table.ExecuteAsync(tableOperation);
        }

        private CloudTableClient GetTableClient()
        {
            var storageAccount = CloudStorageAccount.Parse(ConfigurationManager.AppSettings["AzureWebJobsStorage"]);
            return storageAccount.CreateCloudTableClient();
        }
    }
}