﻿using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace CefSharp.Wpf
{
    internal class DelegateCommand : ICommand
    {
        private readonly Action commandHandler;
        private readonly Func<bool> canExecuteHandler;

        public event EventHandler CanExecuteChanged;

        public DelegateCommand(Action commandHandler, Func<bool> canExecuteHandler = null)
        {
            this.commandHandler = commandHandler;
            this.canExecuteHandler = canExecuteHandler;
        }

        public void Execute(object parameter)
        {
            commandHandler();
        }

        public bool CanExecute(object parameter)
        {
            return
                canExecuteHandler == null || 
                canExecuteHandler();
        }

        public void RaiseCanExecuteChanged()
        {
            if (!Application.Current.Dispatcher.CheckAccess())
            {
                Application.Current.Dispatcher.BeginInvoke((Action) RaiseCanExecuteChanged);
                return;
            }

            if (CanExecuteChanged != null)
            {
                CanExecuteChanged(this, new EventArgs());
            }
        }
    }
}