using System;
using System.Linq;
using BotTemplate.Dialogs;
using BotTemplate.Core.Dialogs;
using Microsoft.Bot.Builder.Dialogs;
using BotTemplate.Core.NaturalLanguage;

namespace BotTemplate.Core.IntentMapper
{
    [Serializable]
    public class DialogIntentMapper
    {
        private readonly IIntentMapper _intentMapper;

        public DialogIntentMapper(IIntentMapper IntentMapper)
        {
            _intentMapper = IntentMapper;
        }

        public IDialog GetDialogForIntent(NaturalLanguageResult NLResult)
        {
            var intent = NLResult.Intents.FirstOrDefault();
            if (intent != null)
            {
                InitialIntent intentMapper = _intentMapper.FindCanHandleIntent(intent);
                if (intentMapper != null)
                {
                    try
                    {
                        return intentMapper(NLResult);
                    }catch(Exception e)
                    {
                        throw e;
                    }
                    
                }
            }
            return new NoneDialog();
        }
    }
}