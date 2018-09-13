using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class OfertaProduto : IDialog
    {
        public async Task StartAsync(IDialogContext context)
        {
            await this.DisplayThumbnailCard(context);
            context.Done("");
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
            return new ThumbnailCard()
            {
                Title = "Oferta relâmpago",
                Subtitle = "Especialmente e únicamente para você",
                Text = "Se comprar agora este produto você terá 10% de desconto.",
                Buttons = new List<CardAction>()
                {
                    new CardAction(ActionTypes.OpenUrl,"Acessar Link",value: "https://netshoes.azurewebsites.net/carrinho.html"){}
                }
            }.ToAttachment();
        }
    }
}