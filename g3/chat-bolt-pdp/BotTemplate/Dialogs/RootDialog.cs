using System;
using System.Linq;
using System.Configuration;
using System.Threading.Tasks;
using Microsoft.Bot.Connector;
using BotTemplate.Core.Dialogs;
using BotTemplate.Core.QnAMaker;
using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.IntentMapper;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.Dialogs
{
    [Serializable]
    public class RootDialog : IRootDialog
    {
        private readonly INaturalLanguageService _NLService;
        private readonly DialogIntentMapper _dialogIntentMapper;
        private readonly IQnAMakerService _qnaService;

        public RootDialog(INaturalLanguageServiceFactory NLServiceFactory, DialogIntentMapper dialogIntentMapper, IQnAMakerService qnaservice)
        {
            _NLService = NLServiceFactory.Create();
            _dialogIntentMapper = dialogIntentMapper;
            _qnaService = qnaservice;
        }

        public async Task StartAsync(IDialogContext context)
        {
            context.Wait(MessageReceivedAsync);
        }

        public async Task MessageReceivedAsync(IDialogContext context, IAwaitable<IMessageActivity> activity)
        {
            if (context.Activity.GetActivityType() == ActivityTypes.ConversationUpdate)
            {
                context.Call(new GreetingsDialog(), AfterCallIsDone);
            }

            else if (context.Activity.GetActivityType() == ActivityTypes.Message)
            {
                var resultActivity = await activity as Activity;
                await SendToLuis(context, resultActivity.Text);
            }
        }

        private async Task SendToLuis(IDialogContext context, string result)
        {
            var nlResult = await _NLService.Query(result);
            context.Call(_dialogIntentMapper.GetDialogForIntent(nlResult), AfterCallIsDone);
        }

        private async Task AfterCallIsDone(IDialogContext context, IAwaitable<object> result)
        {
            var doneMessage = await result;
            if (String.IsNullOrEmpty((string)doneMessage))
            {
                await StartAsync(context);
            }
            else
            {
                await SendToLuis(context, (string)doneMessage);
            }
        }
    }
}