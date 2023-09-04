#!/bin/bash

read -p "Enter your branch name: " branch
read -p "Type 'lp' for the latest pull and 'p' for pushing code: " state

if [[ $state == "lp" ]]; then
  # Merge master into your branch for consistency
  git checkout "${branch}"
  echo "Merging master into ${branch}"
  git merge master
elif [[ $state == "p" ]]; then
  read -p "Comment: " comment
  git add .
  git commit -m "${comment}"
  echo "Pushing code to ${branch}...\n"
  git push origin "${branch}"

  # Check whether you want to merge the current branch into master
  read -p "Do you want to merge ${branch} into master? (yes/no): " in
  if [[ $in == "yes" ]]; then
    echo "Merging into master..."
    git checkout master
    git pull origin master
    git merge "${branch}"
    git push origin master
  fi
fi
