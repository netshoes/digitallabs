using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using BotTemplate.Core.Azure;
using BotTemplate.Core.NaturalLanguage;
using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class SugestaoProdutoDialog : IDialog
    {

        public async Task StartAsync(IDialogContext context)
        {
        }

        private async Task StartouObjetivoCorrida(IDialogContext context, IAwaitable<object> result)
        {
        }

        private List<Attachment> PisadaCard()
        {
            List<Attachment> attachments = new List<Attachment>();
            attachments.Add(new ThumbnailCard()
            {
                Title = "Pronada",
                Subtitle = "Quando a parte de fora do calcanhar toca no chão",
                Text = "o pé inicia uma rotação excessiva para dentro",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/pronada.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Pronada", value: "Pronada") }
            }.ToAttachment());

            attachments.Add(new ThumbnailCard()
            {
                Title = "Supinada",
                Subtitle = "o pé toca o solo com a face externa do calcanhar",
                Text = "o pé toca o solo com a face externa do calcanhar",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/Supinada.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Supinada", value: "Supinada") }
            }.ToAttachment());


            attachments.Add(new ThumbnailCard()
            {
                Title = "Neutra",
                Subtitle = "Começa com a parte externa do calcanhar",
                Text = "o pé rotaciona ligeiramente para dentro",
                Images = new List<CardImage> { new CardImage("https://storagebotbolt.blob.core.windows.net/botfile/Normal.png") },
                Buttons = new List<CardAction>() { new CardAction(ActionTypes.PostBack, "Minha pisada é Neutra", value: "Neutra") }
            }.ToAttachment());

            return attachments;
        }
    }
}