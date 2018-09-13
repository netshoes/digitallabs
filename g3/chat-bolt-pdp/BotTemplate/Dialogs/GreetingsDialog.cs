using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using BotTemplate.Core.Azure;
using BotTemplate.Core.Azure.CosmosDB;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class GreetingsDialog : IDialog
    {
        private readonly CosmosDBProduto _dbProdutos;
        private readonly CosmosDBOferta _dbOferta;
        public GreetingsDialog()
        {
            _dbProdutos = new CosmosDBProduto();
            _dbOferta = new CosmosDBOferta();
        }

        public async Task StartAsync(IDialogContext context)
        {
            try
            {
                await _dbOferta.IncrementaOferta(context.Activity.ChannelId);
                await this.DisplayThumbnailCard(context);

                await Task.Delay(15000);

                await DisplayInatividade(context);

                context.Done("");
            }
            catch (Exception e)
            {
                context.Call(new OfertaProduto(), this.AfterCallIsDone);
            }

        }

        private async Task AfterCallIsDone(IDialogContext context, IAwaitable<object> result)
        {
            await this.DisplayThumbnailCard(context);
        }

        public async Task DisplayThumbnailCard(IDialogContext context)
        {
            var replyMessage = context.MakeMessage();
            Attachment attachments = GetThumbanilCard();
            replyMessage.Attachments = new List<Attachment> { attachments };

            await context.PostAsync(replyMessage);
            context.Done("");
        }

        private static Attachment GetThumbanilCard()
        {
            return new VideoCard
            {
                Title = "MELHORES TÊNIS: ASICS",
                Subtitle = "",
                Text = "Acho que seria legal você olhar esse review. Acredito que pode te ajudar a esclarecer suas dúvidas.",
                Image = null,
                Media = new List<MediaUrl>
            {
                new MediaUrl()
                {
                    Url = "https://www.youtube.com/watch?v=irSPdnMg1K4"
                }
            }
            }.ToAttachment();
        }

        public async Task DisplayInatividade(IDialogContext context)
        {
            var product = _dbProdutos.GetByCode("D18-0650-090");

            var review = product.BestReview;

            var text = new StringBuilder();

            text.AppendLine("Está tudo bem? Percebi que você está com dúvidas, posso te ajudar com alguma coisa?");
            text.AppendLine($"\nVeja o que {review.Nome} disse sobre este item:");
            text.AppendLine($"\n{review.GetFull()}");

            await context.PostAsync(text.ToString());
        }
    }
}