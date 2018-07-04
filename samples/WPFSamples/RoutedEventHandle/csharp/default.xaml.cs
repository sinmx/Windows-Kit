using System;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;

namespace SDKSample
{
    public partial class RoutedEventHandle : StackPanel
    {
		StringBuilder eventstr = new StringBuilder();
        void HandleClick(object sender, RoutedEventArgs args)
        {
            FrameworkElement fe = (FrameworkElement)sender;
            eventstr.Append("Event handled by element named ");
            eventstr.Append(fe.Name);
            eventstr.Append("\n");
            FrameworkElement fe2 = (FrameworkElement)args.Source;
            eventstr.Append("Event originated from source element of type ");
            eventstr.Append(args.Source.GetType().ToString());
            eventstr.Append(" with Name ");
            eventstr.Append(fe2.Name);
            eventstr.Append("\n");
            eventstr.Append("Event used routing strategy ");
            eventstr.Append(args.RoutedEvent.RoutingStrategy);
            eventstr.Append("\n");
            results.Text = eventstr.ToString();
        }
    }
}