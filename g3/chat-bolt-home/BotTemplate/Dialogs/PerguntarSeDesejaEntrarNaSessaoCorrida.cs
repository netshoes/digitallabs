using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class PerguntarSeDesejaEntrarNaSessaoCorrida : IDialog
    {
        private const string URL_REDIRECIONA_CORRIDA = "https://netshoes.azurewebsites.net/busca.html";
        private const string NAO_REDIRECIONAR = "No";

        public object Tipo { get; private set; }

        public async Task StartAsync(IDialogContext context)
        {
            await this.DisplayThumbnailCard(context);
        }

        public async Task DisplayThumbnailCard(IDialogContext context)
        {
            var message = context.MakeMessage();

            var attachment = GetThumbnailCard();
            message.Attachments.Add(attachment);

            await context.PostAsync(message);

            //context.Wait(this.MessageReceivedAsync);
            context.Done(string.Empty);
        }

        public async virtual Task MessageReceivedAsync(IDialogContext context, IAwaitable<IMessageActivity> result)
        {
            var activity = await result as Activity;
            //await context.PostAsync("Ok! Ficaremos por aqui!");
            context.Done(string.Empty);
        }

        private static Attachment GetThumbnailCard()
        {
            var heroCard = new ThumbnailCard
            {
                Title = "Claro! Me siga, clique aqui:",
                Subtitle = string.Empty,//"Para facilitar sua navegação posse te levar até os itens da sessão de corrida!!!",
                Text = string.Empty,
                Buttons = new List<CardAction> {
                    new CardAction(
                        ActionTypes.OpenUrl
                        , "Ir para sessão de corrida!"
                        , value: URL_REDIRECIONA_CORRIDA)
                    //, new CardAction(
                    //    ActionTypes.ImBack
                    //    , "Não"
                    //    , value: NAO_REDIRECIONAR)
                }
            };

            return heroCard.ToAttachment();
        }

    }
}