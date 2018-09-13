using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class GreetingsDialog : IDialog
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
        }

        private static Attachment GetThumbanilCard()
        {
            return new ThumbnailCard()
            {
                Title = "Chat BOLT - Netshoes",
                Subtitle = "Eu sou o melhor assistente, quando o assunto é Esporte",
                Text = "Olá, podemos conversar sobre esportes e posso te dar dicas de produtos.",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/myAvatar.png") }
            }.ToAttachment();
        }

    }
}