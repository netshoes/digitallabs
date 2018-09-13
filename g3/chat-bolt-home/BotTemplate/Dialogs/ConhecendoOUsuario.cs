using Microsoft.Bot.Builder.Dialogs;
using Microsoft.Bot.Connector;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class ConhecendoOUsuario : IDialog
    {
        public object Tipo { get; private set; }

        public async Task StartAsync(IDialogContext context)
        {
            await this.DisplayThumbnailCard(context);
        }

        public async Task DisplayThumbnailCard(IDialogContext context)
        {
            await context.PostAsync($@"me fale um pouco sobre voce, qual esporte poderiamos falar?");
            context.Wait(MessageReceivedAsync);
        }

        private async Task MessageReceivedAsync(IDialogContext context, IAwaitable<object> result)
        {
            var activity = await result as Activity;
            //MOCK
            var esporteDoUsuario = "Corrida";
            await context.PostAsync($@"Certo vamos a ala de {esporteDoUsuario}!");
            //context.Wait(MessageReceivedAsync);
            context.Done("");
        }


    }
}