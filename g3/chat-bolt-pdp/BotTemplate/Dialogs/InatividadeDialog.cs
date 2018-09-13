using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using BotTemplate.Core.Azure;
using BotTemplate.Core.Extensions;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class InatividadeDialog : IDialog
    {
        private readonly CosmosDBProduto _repository;
        private const string ProductCode = "D18-0650-090";

        public InatividadeDialog()
        {
            _repository = new CosmosDBProduto();

        }

        public async Task StartAsync(IDialogContext context)
        {
            await context.PostAsync("Está tudo bem? Gostaria de ver mais ver os comentários de clientes NetShoes sobre esse produto.");

            var product = _repository.GetByCode(ProductCode);

            var review = product.BestReview;
            var message = context.MakeMessage();
            message.AttachmentLayout = AttachmentLayoutTypes.List;
            message.Attachments = new List<Attachment>
            {
                new ThumbnailCard
                {
                    Title = $"{review.Nome.ToUtf8()} disse...",
                    Subtitle = $"Data: {review.Data}",
                    Text = review.GetFull(),
                    Images = new List<CardImage> { new CardImage($"http://static.netshoes.com.br/{product.Images?.Image}") }
                }.ToAttachment()
            };

            await context.PostAsync(message);

            context.Done("");
        }
    }
}