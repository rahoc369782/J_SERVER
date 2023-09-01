read -p "Your branch name?" branch 
read -p "Comment?" comment
git add .
git commit -m "${comment}" .
echo "pushing code to ${branch}..."
git push origin "${branch}"