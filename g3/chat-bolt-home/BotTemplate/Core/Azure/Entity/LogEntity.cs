using System;
using Microsoft.Bot.Connector;
using Microsoft.WindowsAzure.Storage.Table;

namespace BotTemplate.Core.Azure.Entity
{
    public class LogEntity : TableEntity
    {
        public LogEntity(IMessageActivity messageActivity)
        {
            PartitionKey = messageActivity.Conversation.Id;
            RowKey = messageActivity.Id == null ? Guid.NewGuid().ToString() : messageActivity.Id;
            FromUserName = messageActivity.From.Name;
            FromUserId = messageActivity.From.Id;
            ToUserName = messageActivity.Recipient.Name;
            ToUserId = messageActivity.Recipient.Id;
            Message = messageActivity.Text;
            Datetime = DateTime.Now.ToShortDateString();
        }

        public LogEntity() { }

        public string FromUserName { get; set; }
        public string FromUserId { get; set; }
        public string ToUserName { get; set; }
        public string ToUserId { get; set; }
        public string Datetime { get; set; }
        public string Message { get; set; }
    }
}
